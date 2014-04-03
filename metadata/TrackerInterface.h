
#ifndef TRACKER_INTERFACE_H_
#define TRACKER_INTERFACE_H_

#include <string>

#include "MetadataInterface.h"
#include "MetadataRecord.h"

namespace metadata {

// Implement the metadata layer as a centralized tracker for simplicity
class TrackerInterface: public MetadataInterface {
 public:
  TrackerInterface(std::string ip, std::string port);
  MetadataRecord get(std::string key);
  void put(std::string key, MetadataRecord value);

 private:
  std::string serverIP_;
  std::string serverPort_;
}; // class TrackerInterface

} // namespace metadata

#endif // TRACKER_INTERFACE_H_
