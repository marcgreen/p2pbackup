
#define BOOST_NO_CXX11_SCOPED_ENUMS

#include "core/NetworkController.h"
#include "peer/Peer.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <unistd.h>
#include <stdio.h>
#include <exception>
#include <stdexcept>
#include <sstream>
#include <cassert>
#include <cstdlib>
#include <memory>
#include <jsoncpp/json.h>

namespace peer {
  
std::shared_ptr<Peer> Peer::instance_ = std::shared_ptr<Peer>(0);
const int Peer::ENCRYPTION_SECRET_LENGTH = 33;
const float Peer::MAX_BLACKLIST_STORE_RATIO = .25;
const int Peer::TOTAL_REPLICA_COUNT = 5; // TODO change when testing large scale
const std::string Peer::BACKUP_DIR = "backup";
const std::string Peer::STORE_DIR = "store";
const std::string Peer::LOCAL_BACKUP_INFO_FILE = "local_backup_info";
const int Peer::BTSYNC_FOLDER_RESCAN_INTERVAL = 60; // seconds
const int Peer::METADATA_RESCAN_INTERVAL = 60; // seconds
const int Peer::DEFAULT_BTSYNC_PORT = 48247;
const std::string Peer::DEFAULT_BTSYNC_PORT_STR = "48247";
const uint64_t Peer::MINIMUM_STORE_SIZE = 20ull * 1024ull * 1024ull * 1024ull;
const uint32_t Peer::STARTING_NODE_RELIABILITY = 5;

Peer& Peer::constructInstance(std::shared_ptr<metadata::MetadataInterface> metadataI,
			      std::shared_ptr<btsync::BTSyncInterface> btSyncI,
			      std::string btBackupDir,
			      std::string localBackupInfoLocation) {
  if (!instance_)
    instance_ = std::shared_ptr<Peer>(new Peer(metadataI, btSyncI, btBackupDir, localBackupInfoLocation));
  return *instance_;
}

Peer& Peer::getInstance() {
  return *instance_;
}

// private
Peer::Peer(std::shared_ptr<metadata::MetadataInterface> metadataI,
	   std::shared_ptr<btsync::BTSyncInterface> btSyncI,
	   std::string btBackupDir,
	   std::string localBackupInfoLocation) :
  metadataInterface_(metadataI), btSyncInterface_(btSyncI), btBackupDir_(btBackupDir) { 

  // Create root backup directory, btBackupDir, if it's not already present
  createDirIfNeeded(btBackupDir);

  // Create backup and store subdirectories if not already present
  createDirIfNeeded(btBackupDir + "/" + BACKUP_DIR);
  createDirIfNeeded(btBackupDir + "/" + STORE_DIR);
  
  if (!localBackupInfoLocation.empty()) {
    boost::filesystem::copy_file(localBackupInfoLocation,
				 btBackupDir + "/" + LOCAL_BACKUP_INFO_FILE);
  }
  
  // Read in persistent localBackupInfo
  std::unique_lock<std::recursive_mutex> localBackupLock(localInfoMutex_);
  localBackupInfo_.readFromDisk(btBackupDir +"/"+ LOCAL_BACKUP_INFO_FILE);
  
  if (!localBackupInfoLocation.empty())
    recreateAndRegisterBTSyncDirectories();
  
  // Set BTSync preferences
  Json::Value prefs;
  prefs["folder_rescan_interval"] = BTSYNC_FOLDER_RESCAN_INTERVAL;
  prefs["listening_port"] = DEFAULT_BTSYNC_PORT;
  btSyncInterface_->setPreferences(prefs);
}

void Peer::recreateAndRegisterBTSyncDirectories() {
  std::vector<std::string> fileIDs = localBackupInfo_["files"].getMemberNames();
  for (std::string& fileID : fileIDs) {
    std::stringstream ss;
    ss << btBackupDir_ << "/" << BACKUP_DIR << "/" << fileID;
    std::string directory(ss.str());
    createDirIfNeeded(directory);
    btSyncInterface_->addFolder(
      directory, localBackupInfo_["files"][fileID]["rwSecret"].asString());
  }
}

bool Peer::joinNetwork() {
  using namespace std;

  // Calculate our peerID
  string peerID;
  
  if (localBackupInfo_.isMember("ID"))
    peerID = localBackupInfo_["ID"].asString();
  else {
    peerID = btSyncInterface_->getSecrets(true)["encryption"].asString();
    localBackupInfo_["ID"] = peerID;
  }
  
  peerID_ = peerID;
  localBackupInfo_.dumpToDisk(btBackupDir_ + "/" + LOCAL_BACKUP_INFO_FILE);
  
  // Send JOIN command to metadata layer
  try {
    metadataInterface_->joinNetwork(peerID);
  } catch (exception& e) {
    cout << e.what() << endl;
    return false;
  }

  cout << "Successfully joined network" << endl;
  return true;
}

bool Peer::blacklistNode(std::string nodeID) {
  std::cout << "Blacklisting " + nodeID << std::endl;
  try {
    metadataInterface_->blacklistNode(peerID_, nodeID);
  } catch(std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return true;
}

bool Peer::backupFile(std::string path) {
  using namespace std;

  // Generate new BTSync secrets for the file
  Json::Value root = btSyncInterface_->getSecrets(true); // true -> encryption secret
  string rwSecret = root["read_write"].asString();
  string encryptionSecret = root["encryption"].asString();

  // Generate the fileID
  string fileID = encryptionSecret;

  cout << "Backing up file " + path << endl
       << "RW secret: " + rwSecret << endl
       << "Encryption secret: " + encryptionSecret << endl;

  // Make directory in our backup directory to house hard link
  boost::filesystem::path fileIDDir(btBackupDir_ +"/"+ BACKUP_DIR +"/"+ fileID);
  if (!boost::filesystem::create_directory(fileIDDir)) {
    cout << "Error making directory " + fileIDDir.string() << endl;
    return false;
  }
  cout << "Hardlink directory: " + fileIDDir.string() << endl;

  // Create hardlink to file in our backup directory
  boost::filesystem::path originalPath(path);
  string fileName = originalPath.filename().string();
  boost::filesystem::path hardlinkPath = fileIDDir / fileName;
  int err = link(path.c_str(), hardlinkPath.string().c_str());
  if (err != 0) {
    cerr << "Error creating hardlink " + hardlinkPath.string() << endl;
    perror(NULL);
    return false;
  }
  cout << "Hardlink: " + hardlinkPath.string() << endl;

  // Get size of file being backed up for use in finding replicant nodes
  uint64_t filesize = boost::filesystem::file_size(hardlinkPath);
  
  // Acquire access to the LocalBackupInfo data structure until the end of the
  // method.
  std::unique_lock<std::recursive_mutex> localBackupLock(localInfoMutex_);
  // Keep track of filesize locally for synchronization between BTSync and Metadata Layer
  localBackupInfo_["files"][fileID]["size"] = static_cast<Json::UInt64>(filesize);
  localBackupInfo_["files"][fileID]["rwSecret"] = rwSecret;
  if (!localBackupInfo_.dumpToDisk(btBackupDir_ +"/"+ LOCAL_BACKUP_INFO_FILE)) {
    cerr << "Error writing local backup info to disk" << endl;
    return false;
  }

  // Add file to BTSync and set folder preferences
  btSyncInterface_->addFolder(fileIDDir.string(), rwSecret);
  Json::Value params;
  params["use_hosts"] = 1;
  params["use_sync_trash"] = 0;
  // TODO keep this set to 1 b/c otherwise btsync doesn't sync
  //params["use_tracker"] = 0;
  params["use_relay_server"] = 0;
  params["search_lan"] = 0;
  btSyncInterface_->setFolderPreferences(rwSecret, params);
  cout << "Added folder to BTSync and modified preferences" << endl;

  // Replicate file on the network several times
  int numberReplicas = 0;
  cout << "Finding replication nodes..." << endl;
  
  try {
    while (numberReplicas < TOTAL_REPLICA_COUNT)
      if (createReplica(fileID, rwSecret, encryptionSecret, filesize))
	++numberReplicas;
  } catch(std::runtime_error& error) {
    std::cerr << "Backup failed. Reason: " << error.what() << std::endl;
    return false;
  }

  cout << "Completed file backup. Excellent!" << endl;
  return true;
}

bool Peer::createReplica(const std::string& fileID,
			 const std::string& rwSecret,
			 const std::string& encryptionSecret,
			 uint64_t filesize) {
  using namespace std;
  string id;
  
  // Find potential replicant node
  string nodeID; 
  
  try {
    // Get a node ID that is not ours.
    do {
      id = btSyncInterface_->getSecrets(true)["encryption"].asString();
      cout << "\tRandomly generated id: " << id << endl;
      nodeID = metadataInterface_->findClosestNode(id);
    } while (nodeID == peerID_);
  } catch(std::exception& e) {
    std::cerr << e.what() << std::endl;
    return false;
  }
  cout << "\tPotential replicant node: " + nodeID << endl;
  
  // Are we already storing this file on the node?
  Json::Value& nodeArray = localBackupInfo_["files"][fileID]["nodes"];
  for (std::string& currNodeID : nodeArray.getMemberNames())
    if (nodeID == currNodeID) {
      std::cerr << "Node is already being used" << std::endl;
      return false;
    }
  
  // Determine if node is obligated to store file, given the amount they currently backup and store
  metadata::MetadataRecord nodeMetadata;
  try {
    metadataInterface_->get(nodeID, nodeMetadata);
  } catch(std::exception& e) {
    std::cerr << e.what() << std::endl;
    return false;
  }
  uint64_t storedSize = nodeMetadata.getTotalStoreSize();
  uint64_t newStoredSize = filesize + storedSize;
  uint64_t backedUpSize = nodeMetadata.getTotalBackupSize();
  uint64_t obligatedStoreSize = TOTAL_REPLICA_COUNT * backedUpSize;
  cout << "\tNode will be storing: " << to_string(newStoredSize) << endl
       << "\tNode obligated to store: " << to_string(obligatedStoreSize) << endl
       << "\t\t(" + to_string(TOTAL_REPLICA_COUNT) + " * " + to_string(backedUpSize) + ")"
       << endl;
  if (newStoredSize >= obligatedStoreSize && storedSize > MINIMUM_STORE_SIZE)
    return false;
  
  // Ensure the node doesn't have too many blacklisters
  int numBlacklisters = nodeMetadata.getNumberBlacklisters();
  int numStoredFiles = nodeMetadata.getNumberStoredFiles();
  if (numStoredFiles != 0) {
    float blacklistToStoreRatio = numBlacklisters / numStoredFiles;
    cout << "\tNode blacklist:store ratio: "
	 << numBlacklisters << "/" << numStoredFiles
	 << "(" << blacklistToStoreRatio << ")" << endl;
    if (blacklistToStoreRatio > MAX_BLACKLIST_STORE_RATIO)
      return false;
  }
  
  // Ask (tell) node to backup. Wait for ACK, or find other replicant node if they never ACK
  cout << "Node qualifies! Asking to backup...";
  std::string nodeIP = nodeMetadata.getNodeIP();
  if (!askNodeToBackup(nodeIP, encryptionSecret))
    return false;
  cout << "success!" << endl;
  
  // Add file to metadata layer
  cout << "Adding to metadata layer...";
  try {
    metadataInterface_->backupFile(peerID_, nodeID, fileID, filesize);
  } catch (exception& e) {
    cout << "Error adding file to metadata: " << e.what() << endl;
    return false;
  }
  cout << "success!" << endl;
  
  // Tell BTSync how to find the node
  Json::Value hosts = btSyncInterface_->getFolderHosts(rwSecret);
  Json::FastWriter writer;
  hosts["hosts"].append(nodeIP + ":" + to_string(DEFAULT_BTSYNC_PORT));
  cout << "Predefined hosts: " << writer.write(hosts) << endl;
  btSyncInterface_->setFolderHosts(rwSecret, hosts);
  
  // Store relevant data in JSON data structure and write to file
  Json::Value newNode;
  newNode["IP"] = nodeIP;
  newNode["reliability"] = STARTING_NODE_RELIABILITY;
  localBackupInfo_["files"][fileID]["nodes"][nodeID] = newNode;
  if (!localBackupInfo_.dumpToDisk(btBackupDir_ + "/" + LOCAL_BACKUP_INFO_FILE)) {
    throw std::runtime_error("Error writing local backup info to disk");
  }

  cout << "Completed replication to the node. Good job!" << endl;
  return true;
}

bool Peer::storeFile(std::string secret) {
  using namespace std;

  // TODO check that peer doesn't have too many blacklisters

  // Make directory in our store directory to house backup. 'secret' is fileId
  boost::filesystem::path fileIDDir(btBackupDir_ +"/"+ STORE_DIR +"/"+ secret);
  if (!boost::filesystem::create_directory(fileIDDir)) {
    cout << "Error making directory " + fileIDDir.string() << endl;
    return false;
  }
  cout << "Replica directory: " + fileIDDir.string() << endl;

  // Add secret to BTSync
  btSyncInterface_->addFolder(fileIDDir.string(), secret);
  cout << "Added folder to BTSync." << endl;

  // Don't need to do anything with the metadata layer b/c the peer takes care of that

  return true;
}

bool Peer::removeBackup(std::string fileID) {
  using namespace std;

  // Remove file from BTSync (fileID is secret)
  cout << "Removing " << fileID << " from BTSync" << endl;
  Json::Value ret = btSyncInterface_->removeFolder(fileID);
  if (ret["error"] != 0)
    return false;

  // Update metadata layer
  cout << "Updating metadata layer to reflect backup deletion" << endl;
  if (!instance_->updateFileSize(fileID, 0))
    return false;
  // TODO Peer functions return bools and throw exceptions. we should make that consistent. we should
  //   also actually check return values and catch exceptions

  // Delete hardlink and containing directory
  boost::filesystem::path fileDir(btBackupDir_ +"/"+ BACKUP_DIR +"/"+ fileID);
  cout << "Deleting " << fileDir.string() << " to finish backup removal" << endl;
  boost::filesystem::remove_all(fileDir);
  // TODO error check remove_all

  return true;
}

bool Peer::updateFileSize(std::string fileID, uint64_t size) {
  std::unique_lock<std::recursive_mutex> localInfoLock(localInfoMutex_);
  Json::Value& backupNodeList = localBackupInfo_["files"][fileID]["nodes"];
  localInfoLock.unlock();

  /*if (!backupNodeList.isArray())
    throw std::runtime_error("In Peer::updateFile: Malformed LocalBackupInfo "
    "(expected an array)");*/

  //for (int nodeIndex = 0; nodeIndex < backupNodeList.size(); ++nodeIndex) {
  for (std::string& nodeID : backupNodeList.getMemberNames()) {
    try {
      metadataInterface_->updateFileSize(nodeID, fileID, size);
    } catch(std::exception& e) {
      std::cerr << e.what() << std::endl;
      return false;
    }
  }
    
  localBackupInfo_["files"][fileID]["size"] = static_cast<Json::UInt64>(size);
  
  // There isn't anything to indiciate that something went wrong, so just
  // return true
  return true;
}

bool Peer::askNodeToBackup(std::string nodeIP, std::string secret) {
  using boost::asio::ip::tcp;

  if (secret.length() != ENCRYPTION_SECRET_LENGTH)
    throw std::runtime_error("Invalid secret; secrets must be "
			     + std::to_string(ENCRYPTION_SECRET_LENGTH) + 
			     " characters long");

  bool result = false;
  boost::asio::io_service ioService;
  tcp::resolver resolver(ioService);
  tcp::resolver::query query(tcp::v4(), nodeIP, core::CLIENT_PORT_STR);
  tcp::resolver::iterator iterator = resolver.resolve(query);
  tcp::resolver::iterator end;

  tcp::socket socket(ioService);
  boost::system::error_code error = boost::asio::error::host_not_found;
  
  while (error && iterator != end) {
    socket.close();
    socket.connect(*iterator++, error);
  }
  
  if (error)
    throw boost::system::system_error(error);

  try {
    uint8_t msgType = 0;
    boost::asio::write(socket, boost::asio::buffer(&msgType, sizeof(msgType)));
    boost::asio::write(socket, boost::asio::buffer(secret.data(), secret.size()));
    uint8_t nodeAck = 0;
    boost::asio::read(socket, boost::asio::buffer(&nodeAck, sizeof(nodeAck)));
    if (nodeAck != 1)
      throw std::runtime_error("Malformed ACK received from node");
    result = true;
  } catch(boost::system::system_error& error) {
    std::cerr << "boost::system:system_error in Peer::askNodeToBackup: "
	      << error.what() << std::endl;
  } catch(std::runtime_error& error) {
    std::cerr << "std::runtime_error in Peer::askNodeToBackup: "
	      << error.what() << std::endl;
  }
  
  return result;
}

bool Peer::unregisterBackupNode(const std::string& nodeID,
				const std::string& rwSecret) {
  Json::Value currentHosts = btSyncInterface_->getFolderHosts(rwSecret);
  Json::Value newHosts;
  
  for (Json::Value& host : currentHosts["hosts"]) {
    std::string hostAndPort = host.asString();
    std::size_t colonLoc = hostAndPort.find_first_of(':');
    if (colonLoc == std::string::npos) {
      std::cerr << "Peer::unregisterBackupNode - Malformed BTSync data"
		<< std::endl;
      return false; // Malformed data
    }
    std::string hostOnly = hostAndPort.substr(0, colonLoc);
    if (nodeID != hostOnly) {
      newHosts["hosts"].append(hostAndPort);
    }
  }
  btSyncInterface_->setFolderHosts(rwSecret, newHosts);
  return true;
}

void Peer::createDirIfNeeded(std::string path) {
  boost::filesystem::path dir(path);

  if (exists(dir)) {
    if (!is_directory(dir))
      throw std::runtime_error("Supposed directory '" + dir.string() + "' isn't a directory");
  } else {
    std::cout << "Creating directory: " + dir.string() << std::endl;
    if (!boost::filesystem::create_directory(dir))
      throw std::runtime_error("Couldn't create directory: " + dir.string());
  }
}

void Peer::synchronizeWithBTSync() {
  Json::Value folderInfo = btSyncInterface_->getFolders();
  
  // Acquire access to the LocalBackupInfo data structure
  std::unique_lock<std::recursive_mutex> localBackupLock(localInfoMutex_);
  
  for (Json::Value folder : folderInfo) {
    std::string rwSecret = folder["secret"].asString();
    std::string currentFileID = btSyncInterface_->getSecrets(rwSecret, true)["encryption"].asString();
    uint64_t btSyncFileSize = folder["size"].asUInt64();
    uint64_t localInfoFileSize = localBackupInfo_["files"][currentFileID]["size"].asUInt64();
    
    // If the sizes are different, then the file size has changed. Update
    // the LocalBackupInfo data structure and inform the Metadata Layer of the
    // change.
    if (btSyncFileSize != localInfoFileSize) {
      std::cout << "Change in file size found. Information: " << std::endl
		<< "File ID: " << currentFileID << std::endl
		<< "LocalBackupInfo size: " << localInfoFileSize << std::endl
		<< "BTSync size: " << btSyncFileSize << std::endl;
      localBackupInfo_["files"][currentFileID]["size"] = static_cast<Json::UInt64>(btSyncFileSize);
      updateFileSize(currentFileID, btSyncFileSize);
    }
  }
}

void Peer::checkOnBackupNodes() {
  // Get IP address of each node
  // Ping each of them (which will be picked up by their Network Controllers).
  //   Pinging them could consist of opening the socket connection and seeing
  //   if they accept.
  // Wait for a little while. If a node fails to respond within the time limit,
  //   mark them as unreliable.
  // If their unreliable count goes over a threshold, move the replica to a
  //   different node and blacklist that node.
  
  std::vector<std::string> fileIDs;
  
  std::unique_lock<std::recursive_mutex> localBackupLock(localInfoMutex_);
  fileIDs = localBackupInfo_["files"].getMemberNames();
  
  for (std::string fileID : fileIDs) {
    std::list<std::string> toRemove;
    std::vector<std::string> nodeIDs =
      localBackupInfo_["files"][fileID]["nodes"].getMemberNames();
    for (std::string nodeID : nodeIDs) {
      Json::Value& current = localBackupInfo_["files"][fileID]["nodes"][nodeID];
      int status = 0;
      boost::asio::io_service ioService;
      boost::asio::deadline_timer deadline(ioService,
					   boost::posix_time::seconds(5));
      
      std::cout << "checkOnBackupNode: Trying " << current["IP"].asString() << std::endl;
      
      tcp::resolver resolver(ioService);
      tcp::resolver::query query(tcp::v4(),
				 current["IP"].asString(),
				 core::CLIENT_PORT_STR);
      tcp::resolver::iterator iterator = resolver.resolve(query);
      tcp::resolver::iterator end;
      
      tcp::socket s(ioService);
      boost::system::error_code error = boost::asio::error::host_not_found;
      
      while (error && iterator != end) {
	s.close();
	s.connect(*iterator++, error);
      }
      
      std::cout << error.message() << " " << s.is_open() << std::endl;
      
      // If the connection handler wasn't called, then the timeout happened
      // first. In this case, decrement their reliability counter.
      //if (status == 2 || !s.is_open()) {
      if (error || !s.is_open()) {
	// This node dun goofed
	// Consequences will never be the same
	// (Blacklist the node)
	if (current["reliability"].asUInt() == 0) {
	  blacklistNode(nodeID);
	  try {
	    metadataInterface_->removeBackup(peerID_, nodeID, fileID);
	  } catch(std::runtime_error& error) {
	    std::cerr << error.what() << std::endl;
	  }
	  std::string currRWSecret =
	    localBackupInfo_["files"][fileID]["rwSecret"].asString();
	  uint64_t currSize =
	    localBackupInfo_["files"][fileID]["size"].asUInt64();
	  unregisterBackupNode(nodeID, currRWSecret);
	  createReplica(fileID, currRWSecret, fileID, currSize);
	  toRemove.push_back(nodeID);
	  std::cout << "Removing backup on " << nodeID
		    << std::endl;
	} else {
	  current["reliability"] = current["reliability"].asUInt() - 1;
	  std::cout << "Reliability for " << nodeID
		    << "decreased by 1, now set to "
		    << current["reliability"].asUInt()
		    << std::endl;
	  localBackupInfo_.dumpToDisk(btBackupDir_ + "/" + LOCAL_BACKUP_INFO_FILE);
	  std::cout << "checkOnBackupNodes: Done writing to disk" << std::endl;
	}
      } else {
	uint8_t msgType = 1;
	// Send the message type. If that fails, just move on to the next node.
	try {
	  boost::asio::write(s, boost::asio::buffer(&msgType, sizeof(msgType)));
	} catch(boost::system::system_error& error) {
	  continue;
	}
	// Nothing else needs to be sent, so close the connection.
	s.close();
	current["reliability"] = STARTING_NODE_RELIABILITY;
	std::cout << "Reliability for " << nodeID
		  << " reset" << std::endl;
	localBackupInfo_.dumpToDisk(btBackupDir_ + "/" + LOCAL_BACKUP_INFO_FILE);
	std::cout << "checkOnBackupNodes: Done dumping to disk" << std::endl;
      }
    }
    for (std::string& nodeID : toRemove) {
      localBackupInfo_["files"][fileID]["nodes"].removeMember(nodeID);
    }
  }
}

void Peer::checkMetadataForStoreChanges() {
  metadata::MetadataRecord selfRecord;
  // Get our own Metadata Record.
  metadataInterface_->get(peerID_, selfRecord);
  std::set<std::string> storedFileIDs = selfRecord.getStoredFileIDs();
  boost::filesystem::directory_iterator iterator(boost::filesystem::path(btBackupDir_ + "/" + STORE_DIR));
  for(; iterator != boost::filesystem::directory_iterator(); ++iterator) {
    std::string dirName = iterator->path().filename().string();
    if (storedFileIDs.find(dirName) == storedFileIDs.end()) {
      std::cout << "checkMetadataForStoreChange: pruning ID "
		<< dirName << std::endl;
      Json::Value removeResult = btSyncInterface_->removeFolder(dirName);
      // Remove the directory if there was no problem with removing it from
      // BitTorrent Sync.
      if (removeResult["error"].asInt() == 0) {
	boost::filesystem::remove_all(boost::filesystem::path(btBackupDir_ + "/" + STORE_DIR + "/" + dirName));
      }
    }
  }
}

} // namespace peer
