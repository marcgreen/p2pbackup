
#include "metadata/MetadataRecord.h"
#include "tracker/server/TrackerDatabase.h"

#include <algorithm>
#include <cstdint>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <utility>

namespace tracker { namespace server {

std::string TrackerDatabase::findClosest(const std::string& id) {
  std::unique_lock<std::mutex> accessLock(accessMutex_);
  // Start in the middle for binary search
  int min = 0, max = sortedIDs_.size() - 1;
  int currentPos;
  bool found = false, done = false;
  
  std::cout << "Finding closest " << max << std::endl;
  
  while (max >= min && !found) {
    currentPos = (min + max) / 2;
    if (id.compare(sortedIDs_[currentPos]) < 0)
      max = currentPos - 1;
    else if (id.compare(sortedIDs_[currentPos]) > 0)
      min = currentPos + 1;
    else
      found = true;
  }
  
  if (!found) {
    if (id.compare(sortedIDs_[currentPos]) < 0) {
      if (currentPos == 0)
	currentPos = sortedIDs_.size() - 1;
      else
	--currentPos;
    } else {
      currentPos = (currentPos + 1) % sortedIDs_.size();
    }
  }
  
  std::cout << "Found closest " << currentPos << " "
	    << sortedIDs_[currentPos] << std::endl;
  
  return sortedIDs_[currentPos];
}

metadata::MetadataRecord&
TrackerDatabase::getRecord(const std::string& nodeID) {
  std::unique_lock<std::mutex> accessLock(accessMutex_);
  if (records_.count(nodeID) == 0)
    throw std::out_of_range("No record with ID " + nodeID);
  return records_[nodeID];
}

bool TrackerDatabase::join(const std::string& nodeID,
			   const std::string& ipAddress) {
  std::unique_lock<std::mutex> accessLock(accessMutex_);
  records_.insert(std::make_pair
		  (nodeID, metadata::MetadataRecord(ipAddress)));
  sortedIDs_.push_back(nodeID);
  std::sort(sortedIDs_.begin(), sortedIDs_.end());
  return true;
}

bool TrackerDatabase::blacklistNode(const std::string& nodeID,
				    const std::string& blacklisterID) {
  std::unique_lock<std::mutex> accessLock(accessMutex_);
  bool result = false;
  if (records_.count(nodeID) == 1)
    result = records_[nodeID].addBlacklister(blacklisterID, std::time(NULL));
  return result;
}

bool TrackerDatabase::backupFile(const std::string& peerID,
				 const std::string& nodeID,
				 const std::string& fileID,
				 uint64_t size) {
  std::unique_lock<std::mutex> accessLock(accessMutex_);
  bool result = false;
  if (records_.count(peerID) == 1 && records_.count(nodeID) == 1) {
    if (!records_[peerID].addBackupFile(fileID, nodeID, size))
      std::cout << "This is not the first time that fileID " << fileID
		<< " has been backed up" << std::endl;
    result = records_[nodeID].addStoreFile(fileID, peerID, size);
  }
  return result;
}

bool TrackerDatabase::updateFileSize(const std::string& peerID,
				     const std::string& fileID,
				     uint64_t size) {
  std::unique_lock<std::mutex> accessLock(accessMutex_);
  bool result = false;
  if (records_.count(peerID) == 1) {
    result = records_[peerID].updateBackupFileSize(fileID, size);
    std::cout << "result1 = " << result << std::endl;
    for (std::list<metadata::FileMetadata>::iterator backedupNodesIt =
	   records_[peerID].backupNodeIteratorBegin(fileID);
	 backedupNodesIt !=
	   records_[peerID].backupNodeIteratorEnd(fileID);
	 ++backedupNodesIt) {
      result &= records_[(*backedupNodesIt).nodeID].updateStoreFileSize
	(fileID, size);
      std::cout << "result = " << result << std::endl;
    }
  } else {
    std::cerr << "updateFileSize: invalid peer ID" << std::endl;
  }
  return result;
}

bool TrackerDatabase::removeBackup(const std::string& peerID,
				   const std::string& nodeID,
				   const std::string& fileID) {
  std::unique_lock<std::mutex> accessLock(accessMutex_);
  bool result = false;
  if (records_.count(peerID) == 1 && records_.count(nodeID) == 1) {
    result = true;
    if (!records_[peerID].removeBackup(fileID, nodeID)) {
      result = false;
      std::cerr << "MetadataRecord::removeBackup failed" << std::endl;
    }
    if (!records_[nodeID].updateStoreFileSize(fileID, 0)) {
      result = false;
      std::cerr << "MetadataRecord::updateStoreFileSize failed" << std::endl;
    }
  } else {
    std::cerr << "Invalid peer ID or node ID" << std::endl;
  }
  return result;
}

TrackerDatabase::TrackerDatabase() {

}

} } // namespace tracker::server
