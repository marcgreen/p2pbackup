
#ifndef TRACKER_SERVER_TRACKER_DATABASE_H_
#define TRACKER_SERVER_TRACKER_DATABASE_H_

#include <unordered_map>
#include <string>
#include <vector>

namespace metadata {

class MetadataRecord;

} // namespace MetadataRecord

namespace tracker { namespace server {

class TrackerDatabase {
 public:
  const metadata::MetadataRecord& findClosest(const std::string& nodeID);
  const metadata::MetadataRecord& getRecord(const std::string& nodeID);
  bool join(const std::string& nodeID, const std::string& ipAddress);
  bool blackListNode(const std::string& nodeID, uint32_t blacklisterIP);
  bool backupFile(const std::string& nodeID,
		  const std::string& fileID,
		  uint64_t size,
		  const std::string& senderID);
  bool updateFileSize(const std::string& nodeID,
		      const std::string& fileID,
		      uint64_t size,
		      const std::string& peerID);
 private:
  std::unordered_map<std::string, metadata::MetadataRecord> records_;
  std::vector<std::string> sortedIDs_;
}; // class TrackerDatabase

} } // namespace tracker::server

#endif // TRACKER_SERVER_TRACKER_DATABASE_H_
