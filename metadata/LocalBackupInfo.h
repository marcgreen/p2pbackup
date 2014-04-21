
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
  bool isMember(const std::string& memberName) {
    return data_.isMember(memberName);
  }
  Json::Value& operator[](const std::string& key);
 private:
  // The top level keys for this Json::Value will be all the fileIDs we're backing up.
  // Each will have the following subkeys:
  // - "size": self explanatory
  // - "ID": previous ID used by this peer
  // - "nodes": array of nodes storing this file
  /*
    {
      "ID": "some_ID",
      "files": {
        "fileID1": {
	  "size": the_size,
	  "rwSecret": the_read_write_secret,
	  "nodes": {nodeID:{"IP":nodeIP, "reliability":reliability}, ...]
	}
	...
      }
    }
  */
  Json::Value data_;
};

} // namespace metadata

#endif // METADATA_LOCAL_BACKUP_INFO
