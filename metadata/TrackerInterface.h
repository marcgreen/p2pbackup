
#ifndef TRACKER_INTERFACE_H_
#define TRACKER_INTERFACE_H_

#include <string>

#include "MetadataInterface.h"

namespace metadata {

// Implement the metadata layer as a centralized tracker for simplicity
class TrackerInterface: public MetadataInterface {
 public:
  TrackerInterface(std::string ip, std::string port);
  void joinNetwork(std::string nodeID);
  std::string findClosestNode(std::string fileID);
  void get(std::string nodeID, MetadataRecord &metadataRecord);
  void blacklistNode(std::string nodeID);
  void backupFile(std::string nodeID, std::string fileID, uint64_t size);
  void updateFileSize(std::string nodeID, std::string fileID, uint64_t size);

  // Enum to indicate to server what command we are sending
  enum MetadataCommand = { JOIN_NETWORK_CMD = 0, FIND_CLOSEST_NODE_CMD, GET_CMD, 
			   BLACKLIST_NODE_CMD, BACKUP_FILE_CMD, UPDATE_FILE_SIZE_CMD };

  // The size of each command message sent to server
  // Includes a leading byte indicating which command it is
  enum { 
    JOIN_CMD_SIZE = 1 + NODE_ID_BYTES, 
    FIND_CMD_SIZE = 1 + FILE_ID_BYTES,
    GET_CMD_SIZE = 1 + FILE_ID_BYTES, 
    BLACKLIST_CMD_SIZE = 1 + NODE_ID_BYTES,
    BACKUP_CMD_SIZE = 1 + NODE_ID_BYTES + FILE_ID_BYTES + FILESIZE_BYTES,
    UPDATE_CMD_SIZE = 1 + NODE_ID_BYTES + FILE_ID_BYTES + FILESIZE_BYTES
  };

  // The size of each reply message sent from the server
  enum {
    JOIN_REPLY_SIZE = 1,
    FIND_REPLY_SIZE = FILE_ID_BYTES,
    GET_REPLY_SIZE = sizeof(MetadataRecord), 
    BLACKLIST_REPLY_SIZE = 1,
    BACKUP_REPLY_SIZE = 1,
    UPDATE_REPLY_SIZE = 1
  };

 private:
  void sendCommand(Json::Value root);

  std::string serverIP_;
  std::string serverPort_;
}; // class TrackerInterface

} // namespace metadata

#endif // TRACKER_INTERFACE_H_
