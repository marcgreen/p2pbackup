#include "TrackerInterface.h"

#include <string>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <boost/asio.cpp>
#include <jsoncpp/json.h>

namespace metadata {

TrackerInterface::TrackerInterface(std::string ip, std::string port) :
  serverIP_(ip), serverPort_(port) {
    
}

  void TrackerInterface::joinNetwork(std::string nodeID) {
    std::cout << "Joining network as '" << nodeID << "'" << std::endl;

    Json::Value msg;
    root["command"] = JOIN_NETWORK_CMD;
    root["nodeID"] = nodeID;

    Json::Value reply = sendCommand(msg);

    if (reply["error"] != 0) {
      throw std::runtime_error("Error joining network");
    }
  }

  std::string TrackerInterface::findClosestNode(std::string fileID) {
    std::cout << "Finding closest nodeId to '" << key << "'" << std::endl;

    return "TBD";
  }

  void TrackerInterface::get(std::string nodeID, MetadataRecord &metadataRecord) {
    std::cout << "Getting metadata of '" << nodeID << "'" << std::endl;
    
    metadataRecord = MetadataRecord("10.10.10.10");
    // Connect to server, retrieve, and return
    // Populate metadataRecord with the JSON

    return value;
  }


  void TrackerInterface::blacklistNode(std::string nodeID) {
    std::cout << "Blacklisting '" << nodeID << "'" << std::endl;

    // server call
  }

  void TrackerInterface::backupFile(std::string nodeID, std::string fileID, uint64_t size) {
    std::cout << "Inform server we're backing up " << size << " bytes of data "
	      << "identified by '" << fileID << "' to '" << nodeID << "'"
	      << std::endl;

    // server call
  }

  void TrackerInterface::updateFileSize(std::string nodeID, std::string fileID, uint64_t size) {
    std::cout << "Inform server that the data identified by '" << fileID << "', "
	      << "stored on '" << nodeID << "', is now " << size << " bytes"
	      << std::endl;

    // server call
  }

  Json::Value sendCommand(Json::Value root) {
    using boost::asi::ip::tcp;

    Json::FastWriter writer;
    std::string serializedMsg = writer.write(root);
    size_t msgSize = serializedMsg.size() + 1;
    // left off sending serializedMsg as buffer -- may need to convert to cstr
    // may not need the enums defined in TrackerInterface.h anymore
    
    boost::asio::io_service io_service;
    
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(tcp::v4(), serverIP_, serverPort_);
    tcp::resolver::iterator iterator = resolver.resolver(query);

    tcp::socket s(io_server);
    boost::asio::connect(s, iterator);

    boost::asio::write(s, boost::asio::buffer(data, dataSize));

    boost::asio::read(s, boost::asio::buffer(replyData, replySize));
  }

} // Namespace metadata

int main(int argc, char *argv[]) {
  using namespace std;

  string example_key = "39dk3KNJDF9832N";
  metadata::MetadataRecord example_record = metadata::MetadataRecord("2.20.2.20");
  metadata::TrackerInterface tr = metadata::TrackerInterface("127.0.0.1", "6262");

  tr.joinNetwork("jjjjj");
  std::string id = tr.findClosestNode("ccccc");
  cout << "Metadata for " << id << ": " << tr.get(id).toString() << endl;
  tr.blacklistNode("bbbbb");
  tr.backupFile("iiiii", "fffff", 32);
  tr.updateFileSize("iiiii", "fffff", 33);
}
