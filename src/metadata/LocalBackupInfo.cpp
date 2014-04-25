
#include "metadata/LocalBackupInfo.h"

#include <fstream>
#include <boost/filesystem.hpp>

namespace metadata {

LocalBackupInfo::LocalBackupInfo() {
	
}

LocalBackupInfo::LocalBackupInfo(const Json::Value& data) :
  data_(data) {
	
}

bool LocalBackupInfo::dumpToDisk(const std::string& filePath) {
  bool result = false;
  boost::filesystem::path path(filePath);
  std::ofstream oStream(path.string());
	
  if (oStream) {
    Json::FastWriter serializer;
    std::string serializedData = serializer.write(data_);
    oStream << serializedData;
    result = true;
  }
	
  return result;
}

bool LocalBackupInfo::readFromDisk(const std::string& filePath) {
  bool result = false;
  boost::filesystem::path path(filePath);
  std::ifstream iStream(path.string());
	
  if (iStream) {
    Json::Reader reader;
    reader.parse(iStream, data_);
    result = true;
  }
	
  return result;
}

Json::Value& LocalBackupInfo::operator[](const std::string& key) {
  return data_[key];
}

} // namespace metadata
