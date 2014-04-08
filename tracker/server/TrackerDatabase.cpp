
#include "metadata/MetadataRecord.h"
#include "tracker/server/TrackerDatabase.h"

#include <cstdint>
#include <ctime>
#include <utility>

namespace tracker { namespace server {

const metadata::MetadataRecord&
TrackerDatabase::findClosest(const std::string& nodeID) {
  // Start in the middle for binary search
  int min = 0, max = sortedIDs_.size() - 1;
  int currentPos;
  bool found = false, done = false;
  
  while (max >= min && !found) {
    currentPos = (min + max) / 2;
    if (nodeID.compare(sortedIDs_[currentPos]) < 0)
      max = currentPos - 1;
    else if (nodeID.compare(sortedIDs_[currentPos]) > 0)
      min = currentPos + 1;
    else
      found = true;
  }
  
  if (!found) {
    if (nodeID.compare(sortedIDs_[currentPos]) < 0) {
      if (currentPos == 0)
	currentPos = sortedIDs_.size() - 1;
      else
	--currentPos;
    } else {
      currentPos = (currentPos + 1) % sortedIDs_.size();
    }
  }
  
  return records_[sortedIDs_[currentPos]]; // Placeholder for right now
}

const metadata::MetadataRecord&
TrackerDatabase::getRecord(const std::string& nodeID) {
  return records_[nodeID];
}

bool TrackerDatabase::join(const std::string& nodeID,
			   const std::string& ipAddress) {
  bool result = false;
  if (records_.count(nodeID) == 0) {
    result = true;
    records_.insert(std::make_pair
		    (nodeID, metadata::MetadataRecord(ipAddress)));
  }
  return result;
}

bool TrackerDatabase::blackListNode(const std::string& nodeID,
				    uint32_t blacklisterIP) {
  bool result = false;
  if (records_.count(nodeID) == 1)
    ;//result = records_[nodeID].addBlacklister(blacklisterIP, time(0));
  return result;
}

bool TrackerDatabase::backupFile(const std::string& nodeID,
				 const std::string& fileID,
				 uint64_t size,
				 const std::string& peerID) {
  bool result = false;
  if (records_.count(peerID) == 1 && records_.count(nodeID) == 1)
    result = records_[peerID].addBackupFile(fileID, nodeID, size) &&
      records_[nodeID].addStoreFile(fileID, peerID, size);
  return result;
}

bool TrackerDatabase::updateFileSize(const std::string& nodeID,
				     const std::string& fileID,
				     uint64_t size,
				     const std::string& peerID) {
  bool result = false;
  if (records_.count(peerID) == 1 && records_.count(nodeID) == 1)
    result = records_[peerID].updateBackupFileSize(fileID, size) &&
      records_[nodeID].updateStoreFileSize(fileID, size);
  return result;
}

} } // namespace tracker::server
