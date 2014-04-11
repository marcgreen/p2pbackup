
#include "metadata/LocalBackupInfo.h"

#include <fstream>

namespace metadata {

LocalBackupInfo::LocalBackupInfo() {
	
}

LocalBackupInfo::LocalBackupInfo(const Json::Value& data) :
	data_(data) {
	
}

bool LocalBackupInfo::dumpToDisk(const std::string& filePath) {
	bool result = false;
	std::ofstream oStream(filePath);
	
	if (oStream) {
		Json::FastWriter serializer;
		std::string serializedData = serializer.write(data_);
		oStream << serializedData;
		result = true;
	}
	
	return result;
}

Json::Value& LocalBackupInfo::operator[](const std::string& key) {
	return data_[key];
}

} // namespace metadata
