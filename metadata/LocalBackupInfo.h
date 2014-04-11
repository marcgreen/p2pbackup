
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
	Json::Value& operator[](const std::string& key);
 private:
	// There are several top level keys for this Json::Value:
	// 1. backupTo - another Json::Value, where each key is the fileID and the
	//    value is an array of nodeIDs who have a backup.
	Json::Value data_;
};

} // namespace metadata

#endif // METADATA_LOCAL_BACKUP_INFO
