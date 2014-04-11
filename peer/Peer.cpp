
#include "core/NetworkController.h"
#include "peer/Peer.h"

#include <boost/filesystem.hpp>
#include <unistd.h>
#include <stdio.h>
#include <exception>
#include <stdexcept>
#include <sstream>
#include <cassert>
#include <cstdlib>
#include <memory>
#include <openssl/sha.h> // TODO update f/ heartbleed
#include <jsoncpp/json.h>

namespace peer {
  
  std::shared_ptr<Peer> Peer::instance_ = std::shared_ptr<Peer>(0);
  
  Peer& Peer::constructInstance(std::shared_ptr<metadata::MetadataInterface> metadataI,
				std::shared_ptr<btsync::BTSyncInterface> btSyncI,
				std::string btBackupDir) {
    if (!instance_)
      instance_ = std::shared_ptr<Peer>(new Peer(metadataI, btSyncI, btBackupDir));
    return *instance_;
  }

  Peer& Peer::getInstance() {
    return *instance_;
  }

  // private
  Peer::Peer(std::shared_ptr<metadata::MetadataInterface> metadataI,
						 std::shared_ptr<btsync::BTSyncInterface> btSyncI,
						 std::string btBackupDir) :
    metadataInterface_(metadataI), btSyncInterface_(btSyncI), btBackupDir_(btBackupDir) { 
	}

  bool Peer::joinNetwork() {
    // Calculate our peerID
    // Address space is 160bits, defined by length of BTSync's encryption secret (used as fileID)
    std::string peerID = btSyncInterface_->getSecrets(true)["encryption"].asString();
    peerID_ = peerID;

    // Send JOIN command to metadata layer
    try {
      metadataInterface_->joinNetwork(peerID);
    } catch (std::exception& e) {
      std::cout << e.what() << std::endl;
    }
  }

  bool Peer::blacklistNode(std::string nodeID) {
    metadataInterface_->blacklistNode(peerID_, nodeID);
  }

  bool Peer::backupFile(std::string path) {
    /*// Generate new BTSync secrets for the file
    Json::Value root = btSyncInterface_->getSecrets(true); // true -> encryption secret
    std::string rwSecret = root["read_write"].asString();
    std::string encryptionSecret = root["encryption"].asString();

    // Generate the fileID
    std::string fileID = encryptionSecret;

    // Create btBackupDir if it's not already present TODO

    // Make directory in our backup directory to house hard link
    boost::filesystem::path fileIDDir(btBackupDir_ + "/" + BACKUP_DIR + "/" + fileID);
    if (!boost::filesystem::create_directory(fileIDDir)) {
      std::cout << "Error making directory " + fileIDDir.string() << std::endl;
      return false;
    }
    std::cout << "Made dir " + fileIDDir.string() << std::endl;

    // Create hardlink to file in our backup directory
    boost::filesystem::path originalPath(path);
    std::string fileName = originalPath.filename().string();
    boost::filesystem::path hardlinkPath(fileIDDir.string() + "/" + fileName);
    int err = link(path.c_str(), hardlinkPath.string().c_str());
    if (err != 0) {
      std::cout << "Error creating hardlink " + hardlinkPath.string() << std::endl;
      perror(NULL);
      return false;
    }
    std::cout << "Made hardlink " + hardlinkPath.string() << std::endl;

    // Get size of file being backed up for use in finding replicant nodes
    uint64_t filesize = boost::filesystem::file_size(hardlinkPath);

    // Add file to BTSync
    btSyncInterface_->addFolder(fileIDDir.string(), rwSecret);

    // Replicate file on the network several times
    int numberReplicas = 0;
    while (numberReplicas < TOTAL_REPLICA_COUNT) {
      std::string salt = Peer::salt();
      std::string id = sha256String(fileID + salt);
      
      // Find potential replicant node
      std::string nodeID = metadataInterface_->findClosestNode(id);

      // Determine if node is obligated to store file, given the amount they currently backup and store
      metadata::MetadataRecord nodeMetadata;
      metadataInterface_->get(nodeID, nodeMetadata);
      if (filesize + nodeMetadata.getTotalStoreSize() >=
	  TOTAL_REPLICA_COUNT * nodeMetadata.getTotalBackupSize())
	continue;

      // Ensure the node doesn't have too many blacklisters
      float blacklistToStoreRatio = nodeMetadata.getNumberBlacklisters() / nodeMetadata.getNumberStoredFiles(); 
      if (blacklistToStoreRatio > MAX_BLACKLIST_STORE_RATIO)
	continue;
	
      // Ask (tell) node to backup. Wait for ACK, or find other replicant node if they never ACK
      if (!askNodeToBackup(nodeMetadata.getNodeIP(), encryptionSecret))
	continue;
          
      // Add file to metadata layer
      try {
	metadataInterface_->backupFile(nodeID, fileID, filesize);
      } catch (std::exception& e) {
	std::cout << e.what() << std::endl;
      }

      // Store relevant data in JSON data structure and write to file 

      numberReplicas++;
			}*/
		return true;
  }

  bool Peer::storeFile(std::string secret) {
    // create direcotry under btBackupDir/STORE_DIR and name it fileID
  }

  bool Peer::removeBackup(std::string path) {

  }

  bool Peer::updateFileSize(std::string fileID, uint64_t size) {
		Json::Value& backupNodeList = localBackupInfo_["backupTo"][fileID];
		
		if (!backupNodeList.isArray())
			throw std::runtime_error("In Peer::updateFile: Malformed SimpleMetaInfo "
															 "(expected an array)");
		
		for (int nodeIndex = 0; nodeIndex < backupNodeList.size(); ++nodeIndex)
			metadataInterface_->updateFileSize(
				backupNodeList[nodeIndex], fileID, size);
		
		// There isn't anything to indiciate that something went wrong, so just
		// return true
		return true;
  }
  
  bool Peer::askNodeToBackup(std::string nodeIP, std::string secret) {
    using boost::asio::ip::tcp;
		
    // All secrets must be 20 characters long
    if (secret.length() != 20)
      throw std::runtime_error("Invalid secret; secrets must be "
			       "20 characters long");
		
		bool result = false;
		boost::asio::io_service ioService;
		tcp::resolver resolver(ioService);
		tcp::resolver::query query(tcp::v4(), nodeIP, core::CLIENT_PORT_STR);
		tcp::resolver::iterator iterator = resolver.resolve(query);
		
    tcp::socket socket(ioService);
		
		try {
			boost::asio::write(socket, boost::asio::buffer(secret.data(), 20));
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

  std::string Peer::sha256String(std::string input) {
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)input.c_str(), input.size(), digest);
    
    std::stringstream output;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
      output << std::hex << (int)digest[i];
    }

    return output.str();
  }

  std::string Peer::sha256File(std::string path) {
    FILE *f;
    unsigned char buf[8192]; // read 8kb at a time
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256_CTX sc;
    int err;

    f = fopen(path.c_str(), "rb");
    if (f == NULL)
      throw std::runtime_error("Couldn't open file at: " + path);

    if (!SHA256_Init(&sc))
      throw std::runtime_error("Error SHA256 Init");

    for (;;) {
      size_t len = fread(buf, 1, sizeof buf, f);
      if (len == 0)
	break;

      if(!SHA256_Update(&sc, buf, len))
	throw std::runtime_error("Error SHA256 Update");
    }

    err = ferror(f);
    fclose(f);
    if (err)
      throw std::runtime_error("Couldn't close file. Error code: " + err);
    
    if (!SHA256_Final(digest, &sc))
      throw std::runtime_error("Error SHA256 Final");

    std::stringstream output;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
      output << std::hex << (int)digest[i];
    }

    return output.str();
  }

  std::string Peer::salt() {
    return std::to_string(rand());
  }

} // namespace peer
