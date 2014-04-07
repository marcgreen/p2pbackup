
#include "metadata/MetadataRecord.h"
#include "tracker/TrackerDatabase.h"

namespace tracker {

const metadata::MetadataRecord&
TrackerDatabase::findClosest(const std::string& nodeID) {
  return records_[""]; // Placeholder for right now
}

} // namespace tracker
