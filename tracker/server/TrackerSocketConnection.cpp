
// TODO: backup and update need to be one operation that edit both the peer
//       and the node. Also, make who you are backing up to a list (this
//       change is to be made in MetadataRecord)

#include "metadata/MetadataRecord.h"
#include "tracker/TrackerProtocol.h"
#include "tracker/server/TrackerDatabase.h"
#include "tracker/server/TrackerSocketConnection.h"

#include <boost/lexical_cast.hpp>
#include <cstdint>
#include <iostream>
#include <vector>

namespace tracker { namespace server {

void sendWrapper(const Json::Value& dataToSend,
		 tcp::socket& socket,
		 const std::string& source) {
  bool sendSuccessful = tracker::send(dataToSend, socket);
  
  if (!sendSuccessful)
    std::cerr << "Failed to send data back to peer, from "
	      << source << std::endl;
}

void handleTrackerSocketConnection(std::shared_ptr<tcp::socket> socket) {
  Json::Value value;
  recv(value, *socket);
  
  tracker::TrackerCommand command =
    static_cast<tracker::TrackerCommand>(value["command"].asInt());
  TrackerDatabase& trackerDatabase = TrackerDatabase::getInstance();
  
  switch (command) {
  case JOIN_NETWORK_CMD:
    handleJoin(socket, value, trackerDatabase);
    break;
  case FIND_CLOSEST_NODE_CMD:
    handleFindClosestNode(socket, value, trackerDatabase);
    break;
  case GET_CMD:
    handleGet(socket, value, trackerDatabase);
    break;
  case BLACKLIST_NODE_CMD:
    handleBlacklist(socket, value, trackerDatabase);
    break;
  case BACKUP_FILE_CMD:
    handleBackup(socket, value, trackerDatabase);
    break;
  case UPDATE_FILE_SIZE_CMD:
    handleUpdateFileSize(socket, value, trackerDatabase);
    break;
  default:
    std::cerr << "Unknown message type "
	      << command << " received" << std::endl;
  }
}

void handleJoin(std::shared_ptr<tcp::socket> socket,
		Json::Value& networkData,
		TrackerDatabase& trackerDatabase) {
  const std::string clientIP = socket->remote_endpoint().address().to_string();
  const std::string clientID = networkData["nodeID"].asString();
  bool result = trackerDatabase.join(clientID, clientIP);
  
  Json::Value returnValue;
  returnValue["error"] = static_cast<int>(!result);
  sendWrapper(returnValue, *socket, "handleJoin");
}

void handleFindClosestNode(std::shared_ptr<tcp::socket> socket,
			   Json::Value& networkData,
			   TrackerDatabase& trackerDatabase) {
  const std::string closeID = networkData["id"].asString();
  const std::string closestFound = trackerDatabase.findClosest(closeID);
  
  Json::Value returnValue;
  returnValue["error"] = 0;
  returnValue["nodeID"] = closestFound;
  
  sendWrapper(returnValue, *socket, "handleFindClosestNode");
}

void handleGet(std::shared_ptr<tcp::socket> socket,
	       Json::Value& networkData,
	       TrackerDatabase& trackerDatabase) {
  const std::string nodeID = networkData["nodeID"].asString();
  Json::Value returnValue;
  
  try {
    const std::string serializedRecord =
      trackerDatabase.getRecord(nodeID).serialize();
    returnValue["error"] = 0;
    returnValue["metadata"] = serializedRecord;
  } catch(const std::out_of_range& e) {
    std::cerr << e.what() << std::endl;
    returnValue["error"] = 1;
  }
  
  sendWrapper(returnValue, *socket, "handleGet");
}

void handleBlacklist(std::shared_ptr<tcp::socket> socket,
		     Json::Value& networkData,
		     TrackerDatabase& trackerDatabase) {
  const std::string nodeID = networkData["nodeID"].asString();
  const std::string blacklisterID = networkData["peerID"].asString();
  bool commandResult = trackerDatabase.blacklistNode(nodeID, blacklisterID);
  
  Json::Value returnValue;
  returnValue["error"] = static_cast<int>(commandResult);
  sendWrapper(returnValue, *socket, "handleBlacklist");
}

void handleBackup(std::shared_ptr<tcp::socket> socket,
		  Json::Value& networkData,
		  TrackerDatabase& trackerDatabase) {
  const std::string peerID = networkData["peerID"].asString();
  const std::string nodeID = networkData["nodeID"].asString();
  const std::string fileID = networkData["fileID"].asString();
  uint64_t size = networkData["size"].asUInt64();
  bool commandResult =
    trackerDatabase.backupFile(peerID, nodeID, fileID, size);
  
  Json::Value returnValue;
  returnValue["error"] = static_cast<int>(commandResult);
  sendWrapper(returnValue, *socket, "handleBackup");
}

void handleUpdateFileSize(std::shared_ptr<tcp::socket> socket,
			  Json::Value& networkData,
			  TrackerDatabase& trackerDatabase) {
  const std::string peerID = networkData["peerID"].asString();
  const std::string fileID = networkData["fileID"].asString();
  uint64_t size = networkData["size"].asUInt64();
  bool commandResult =
    trackerDatabase.updateFileSize(peerID, fileID, size);
  
  Json::Value returnValue;
  returnValue["error"] = static_cast<int>(commandResult);
  sendWrapper(returnValue, *socket, "handleUpdateFileSize");
}

} } // namespace tracker::server
