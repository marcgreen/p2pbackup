
#ifndef TRACKER_TRACKER_DATABASE_H_
#define TRACKER_TRACKER_DATABASE_H_

#include <unordered_map>
#include <string>
#include <vector>

namespace metadata {

class MetadataRecord;

} // namespace MetadataRecord

namespace tracker {

class TrackerDatabase {
 public:
  const metadata::MetadataRecord& findClosest(const std::string& nodeID);
 private:
  // TODO: MetadataRecord needs a default constructor
  std::unordered_map<std::string, metadata::MetadataRecord> records_;
  std::vector<std::string> sortedIDs_;
}; // class TrackerDatabase

} // namespace tracker

#endif // TRACKER_TRACKER_DATABASE_H_
