
#include <utility>
#include <string>
#include <jsoncpp/json.h>
#include <boost/algorithm/string.hpp>

#include "metadata/MetadataRecord.h"

namespace metadata {

MetadataRecord::MetadataRecord() {}

MetadataRecord::MetadataRecord(std::string ip) :
  nodeIP_(ip) { }

bool MetadataRecord::addBlacklister(std::string peerID, time_t timestamp) {
  auto v = blacklisters_.insert(std::make_pair(peerID, timestamp));
  return v.second;
}

uint64_t MetadataRecord::getTotalBackupSize() {
  uint64_t size = 0;

  for (auto& v : backedUpFiles_) {
    size += v.second.size();
  }

  return size;
}

bool MetadataRecord::addBackupFile(std::string fileID, std::string nodeID, uint64_t size) {
  if (backedUpFiles_.count(fileID) == 1) {
    backedUpFiles_[fileID].push_back(FileMetadata(nodeID, size, std::time(NULL)));
    return false;
  } else {
    backedUpFiles_[fileID].push_back(FileMetadata(nodeID, size, std::time(NULL)));
    return true;
  }
}

bool MetadataRecord::updateBackupFileSize(std::string fileID, uint64_t size) {
  if (size == 0) {
    return backedUpFiles_.erase(fileID) == 1;
  } else {
    for (std::list<FileMetadata>::iterator backupNodeIt =
	   backedUpFiles_[fileID].begin();
	 backupNodeIt != backedUpFiles_[fileID].end();
	 ++backupNodeIt)
      (*backupNodeIt).size = size;
    return true;
  }
}

bool MetadataRecord::removeBackup(const std::string& fileID,
				  const std::string& nodeID) {
  bool result = false;
  if (backedUpFiles_.count(fileID)) {
    int preOpSize = backedUpFiles_[fileID].size();
    backedUpFiles_[fileID].remove_if(
      [&nodeID](const FileMetadata& data) -> bool {
	std::cout << "MetadataRecord::removeBackup compare: "
		  << "data.nodeID = " << data.nodeID
		  << ", nodeID = " << nodeID << std::endl;
	return data.nodeID == nodeID;
      });
    int postOpSize = backedUpFiles_[fileID].size();
    result = (preOpSize == (postOpSize + 1));
    if (!result)
      std::cerr << "in MetadataRecord::removeBackup: preOpSize = " << preOpSize
		<< ", postOpSize = " << postOpSize << std::endl;
  }
  return result;
}

uint64_t MetadataRecord::getTotalStoreSize() {
  uint64_t size = 0;

  for (auto& v : storedFiles_) {
    size += v.second.size;
  }

  return size;
}

bool MetadataRecord::addStoreFile(std::string fileID, std::string peerID, uint64_t size) {
  if (storedFiles_.count(fileID) == 1) {
    return false;
  } else {
    auto v = storedFiles_.insert(std::make_pair(fileID, FileMetadata(peerID, size, std::time(NULL))));
    return v.second;
  }
}

bool MetadataRecord::updateStoreFileSize(std::string fileID, uint64_t size) {
  if (size == 0) {
    return storedFiles_.erase(fileID) == 1;
  } else {
    storedFiles_[fileID].size = size;
    return true;
  }
}

std::string MetadataRecord::serialize() {
  Json::Value root;
  Json::FastWriter writer;

  root["nodeIP"] = nodeIP_;

  for (auto& el : blacklisters_) {
    // Prune blacklisters if their entry was inserted more than PRUNE_AGE seconds ago
    if (PRUNE_AGE >= std::time(NULL) - el.second) {
      Json::Value node;
      node["nodeID"] = el.first;
      node["timestamp"] = static_cast<Json::Int>(el.second);

      root["blacklisters"].append(node);
    } else {
      blacklisters_.erase(el.first);
    }
  }

  for (auto& el : backedUpFiles_) {
    Json::Value backedUpFileList;
    std::string fileID = el.first;
    
    for (std::list<FileMetadata>::iterator backupNodeIt =
	   el.second.begin();
	 backupNodeIt != el.second.end();
	 ++backupNodeIt) {
      Json::Value backupEntry;
      backupEntry["nodeID"] = (*backupNodeIt).nodeID;
      backupEntry["size"] = static_cast<Json::UInt64>((*backupNodeIt).size);
      backupEntry["timestamp"] = static_cast<Json::Int>((*backupNodeIt).timestamp);
      backedUpFileList.append(backupEntry);
    }
    
    root["backedUpFiles"][fileID] = backedUpFileList;
  }

  for (auto& el : storedFiles_) {
    Json::Value node;
    node["fileID"] = el.first;
    node["fileMetadata"]["nodeID"] = el.second.nodeID;
    node["fileMetadata"]["size"] = static_cast<Json::UInt64>(el.second.size);
    node["fileMetadata"]["timestamp"] = std::to_string(el.second.timestamp);

    root["storedFiles"].append(node);
  }

  return writer.write(root);
}

bool MetadataRecord::unserialize(std::string reply) {
  Json::Value root;
  Json::Reader reader;

  bool parsingSuccessful = reader.parse(reply, root);
  if (!parsingSuccessful) {
    std::cout  << "Failed to parse configuration\n";
    return false;
  }

  nodeIP_ = root["nodeIP"].asString();

  for (Json::Value el : root["blacklisters"]) {
    addBlacklister(el["nodeID"].asString(), el["timestamp"].asInt());
  }

  for (std::string el : root["backedUpFiles"].getMemberNames()) {
    for (Json::Value subel : root["backedUpFiles"][el]) {
      backedUpFiles_[el].push_back
	(FileMetadata(subel["nodeID"].asString(),
		      subel["size"].asUInt64(),
		      subel["timestamp"].asInt()));
    }
  }

  for (Json::Value el : root["storedFiles"]) {
    storedFiles_.insert(std::make_pair(el["fileID"].asString(),
				       FileMetadata(el["nodeID"].asString(),
						    el["size"].asUInt64(),
						    el["timestamp"].asInt())));
  }

    return true;
}

std::list<FileMetadata>::iterator MetadataRecord::backupNodeIteratorBegin
  (const std::string& fileID) {
  return backedUpFiles_[fileID].begin();
}

std::list<FileMetadata>::iterator MetadataRecord::backupNodeIteratorEnd
  (const std::string& fileID) {
  return backedUpFiles_[fileID].end();
}

std::set<std::string> MetadataRecord::getStoredFileIDs() {
  std::map<std::string, FileMetadata>::iterator storedIt;
  std::map<std::string, FileMetadata>::iterator storedEnd =
    storedFiles_.end();
  std::set<std::string> result;
  
  for (storedIt = storedFiles_.begin(); storedIt != storedEnd; storedIt++)
    result.insert(storedIt->first);
  
  return result;
}

} // namespace metadata
