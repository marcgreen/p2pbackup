
#ifndef TRACKER_INTERFACE_H_
#define TRACKER_INTERFACE_H_

#include <string>
#include <jsoncpp/json.h>

#include "metadata/MetadataInterface.h"

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

  private:
  std::string executeCommand(const Json::Value &msg, Json::Value &reply);

  std::string serverIP_;
  std::string serverPort_;
}; // class TrackerInterface

} // namespace metadata

#endif // TRACKER_INTERFACE_H_
