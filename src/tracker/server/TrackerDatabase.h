
#ifndef TRACKER_SERVER_TRACKER_DATABASE_H_
#define TRACKER_SERVER_TRACKER_DATABASE_H_

#include "metadata/MetadataRecord.h"

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace tracker { namespace server {

class TrackerDatabase {
 public:
  static TrackerDatabase& getInstance() {
    static TrackerDatabase instance;
    return instance;
  }
  
  std::string findClosest(const std::string& id);
  metadata::MetadataRecord& getRecord(const std::string& nodeID);
  bool join(const std::string& nodeID, const std::string& ipAddress);
  bool blacklistNode(const std::string& nodeID,
		     const std::string& blacklisterID);
  bool backupFile(const std::string& peerID,
		  const std::string& nodeID,
		  const std::string& fileID,
		  uint64_t size);
  bool updateFileSize(const std::string& peerID,
		      const std::string& fileID,
		      uint64_t size);
  bool removeBackup(const std::string& peerID,
		    const std::string& nodeID,
		    const std::string& fileID);
 private:
  TrackerDatabase();
  
  std::unordered_map<std::string, metadata::MetadataRecord> records_;
  std::vector<std::string> sortedIDs_;
  std::mutex accessMutex_;
}; // class TrackerDatabase

} } // namespace tracker::server

#endif // TRACKER_SERVER_TRACKER_DATABASE_H_
