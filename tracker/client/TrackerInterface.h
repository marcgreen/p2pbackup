
#ifndef TRACKER_CLIENT_TRACKER_INTERFACE_H_
#define TRACKER_CLIENT_TRACKER_INTERFACE_H_

#include <string>
#include <jsoncpp/json.h>

#include "metadata/MetadataInterface.h"

namespace tracker { namespace client {

// Implement the metadata layer as a centralized tracker for simplicity
class TrackerInterface: public metadata::MetadataInterface {
 public:
  TrackerInterface(std::string ip, std::string port);
  void joinNetwork(std::string nodeID);
  std::string findClosestNode(std::string fileID);
  void get(std::string nodeID, metadata::MetadataRecord &metadataRecord);
  void blacklistNode(std::string nodeID);
  void backupFile(std::string nodeID, std::string fileID, uint64_t size);
  void updateFileSize(std::string nodeID, std::string fileID, uint64_t size);

  private:
  std::string executeCommand(const Json::Value &msg, Json::Value &reply);

  std::string serverIP_;
  std::string serverPort_;
}; // class TrackerInterface

} } // namespace tracker::client

#endif // TRACKER_CLIENT_TRACKER_INTERFACE_H_