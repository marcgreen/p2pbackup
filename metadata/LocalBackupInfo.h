
#ifndef METADATA_LOCAL_BACKUP_INFO
#define METADATA_LOCAL_BACKUP_INFO

#include <jsoncpp/json.h>
#include <string>

namespace metadata {

class LocalBackupInfo {
 public:
  LocalBackupInfo();
  LocalBackupInfo(const Json::Value& data);
  bool dumpToDisk(const std::string& filePath);
  bool readFromDisk(const std::string& filePath);
  Json::Value& operator[](const std::string& key);
 private:
  // The top level keys for this Json::Value will be all the fileIDs we're backing up.
  // Each will have the following subkeys:
  // - "size": self explanatory
  // - "nodes": array of nodes storing this file
  Json::Value data_;
};

} // namespace metadata

#endif // METADATA_LOCAL_BACKUP_INFO
