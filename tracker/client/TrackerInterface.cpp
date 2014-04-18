#include "metadata/MetadataRecord.h"
#include "tracker/client/TrackerInterface.h"
#include "tracker/TrackerProtocol.h"

#include <string>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <boost/asio.hpp>
#include <jsoncpp/json.h>

namespace tracker { namespace client {

TrackerInterface::TrackerInterface(std::string ip, std::string port) :
  serverIP_(ip), serverPort_(port) {
    
}

void TrackerInterface::joinNetwork(std::string nodeID) {
  std::cout << "Joining network as '" << nodeID << "'" << std::endl;

  Json::Value msg, reply;
  msg["command"] = tracker::JOIN_NETWORK_CMD;
  msg["nodeID"] = nodeID;

  std::string error = executeCommand(msg, reply);
  if (!error.empty()) throw std::runtime_error(error);

  if (reply["error"] == 1) throw std::runtime_error("Server says error joining network");
}

std::string TrackerInterface::findClosestNode(std::string id) {
  std::cout << "Finding closest nodeId to '" << id << "'" << std::endl;

  Json::Value msg, reply;
  msg["command"] = tracker::FIND_CLOSEST_NODE_CMD;
  msg["id"] = id;

  std::string error = executeCommand(msg, reply);
  if (!error.empty()) throw std::runtime_error(error);

  if (reply["error"] == 1) throw std::runtime_error("Server says error finding closest node");

  return reply["nodeID"].asString();
}

void TrackerInterface::get(std::string nodeID, metadata::MetadataRecord &metadataRecord) {
  std::cout << "Getting metadata of '" << nodeID << "'" << std::endl;

  Json::Value msg, reply;
  msg["command"] = tracker::GET_CMD;
  msg["nodeID"] = nodeID;

  std::string error = executeCommand(msg, reply);
  if (!error.empty()) throw std::runtime_error(error);

  if (reply["error"] == 1) throw std::runtime_error("Server says error getting node metadata");

  if (!metadataRecord.unserialize(reply["metadata"].asString())) {
    throw std::runtime_error("Error unserializing JSON from get reply");
  }
}


void TrackerInterface::blacklistNode(std::string peerID, std::string nodeID) {
  std::cout << "Blacklisting '" << nodeID << "'" << std::endl;

  Json::Value msg, reply;
  msg["command"] = tracker::BLACKLIST_NODE_CMD;
  msg["peerID"] = peerID;
  msg["nodeID"] = nodeID;

  std::string error = executeCommand(msg, reply);
  if (!error.empty()) throw std::runtime_error(error);

  if (reply["error"] == 1) throw std::runtime_error("Server says error blacklisting node");
}

void TrackerInterface::backupFile(std::string peerID, std::string nodeID, std::string fileID, uint64_t size) {
  std::cout << "Inform server we're backing up " << size << " bytes of data "
	    << "identified by '" << fileID << "' to '" << nodeID << "'"
	    << std::endl;

  Json::Value msg, reply;
  msg["command"] = tracker::BACKUP_FILE_CMD;
  msg["peerID"] = peerID;
  msg["nodeID"] = nodeID;
  msg["fileID"] = fileID;
  msg["size"] = static_cast<Json::UInt64>(size);

  std::string error = executeCommand(msg, reply);
  if (!error.empty()) throw std::runtime_error(error);

  if (reply["error"] == 1) throw std::runtime_error("Server says error recording data of backed up file");
}

void TrackerInterface::updateFileSize(std::string peerID, std::string fileID, uint64_t size) {
  std::cout << "Inform server that the data identified by '" << fileID << "', "
	    << "backed up by '" << peerID << "', is now " << size << " bytes"
	    << std::endl;

  Json::Value msg, reply;
  msg["command"] = tracker::UPDATE_FILE_SIZE_CMD;
  msg["peerID"] = peerID;
  msg["fileID"] = fileID;
  msg["size"] = static_cast<Json::UInt64>(size);

  std::string error = executeCommand(msg, reply);
  if (!error.empty()) throw std::runtime_error(error);

  if (reply["error"] == 1) throw std::runtime_error("Server says error updating file size");

}

void TrackerInterface::removeBackup(std::string peerID,
				    std::string nodeID,
				    std::string fileID) {
  std::cout << "Removing backup of file ID " << fileID
	    << "on node ID " << nodeID << std::endl;
  
  Json::Value msg, reply;
  msg["command"] = tracker::REMOVE_BACKUP_CMD;
  msg["peerID"] = peerID;
  msg["nodeID"] = nodeID;
  msg["fileID"] = fileID;

  std::string error = executeCommand(msg, reply);
  if (!error.empty()) throw std::runtime_error(error);

  if (reply["error"] == 1) throw std::runtime_error("Failure to remove backup");
}

std::string TrackerInterface::executeCommand(const Json::Value &msg, Json::Value &reply) {
  using boost::asio::ip::tcp;

  boost::asio::io_service io_service;

  tcp::resolver resolver(io_service);
  tcp::resolver::query query(tcp::v4(), serverIP_, serverPort_);
  tcp::resolver::iterator iterator = resolver.resolve(query);
  tcp::resolver::iterator end;

  tcp::socket s(io_service);
  boost::system::error_code error = boost::asio::error::host_not_found;
  
  while (error && iterator != end) {
    s.close();
    s.connect(*iterator++, error);
  }
  
  if (error)
    throw boost::system::system_error(error);

  if (!tracker::send(msg, s)) return "Error sending to server";
  if (!tracker::recv(reply, s)) return "Error recving from server";

  return "";
}

} } // Namespace tracker::client

/*int main(int argc, char *argv[]) {
  using namespace std;

  string example_key = "39dk3KNJDF9832N";
  metadata::MetadataRecord example_record = metadata::MetadataRecord("2.20.2.20");
  tracker::client::TrackerInterface tr = tracker::client::TrackerInterface("127.0.0.1", "6262");

  metadata::MetadataRecord reply;
  tr.joinNetwork("jjjjj");
  std::string id = tr.findClosestNode("ccccc");
  tr.get(id, reply);
  cout << "Metadata for " << id << ": " << reply.toString() << endl;
  tr.blacklistNode("jjjjj", "bbbbb");
  tr.backupFile("iiiii", "fffff", 32);
  tr.updateFileSize("iiiii", "fffff", 33);
	}*/
