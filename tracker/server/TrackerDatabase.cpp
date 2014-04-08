
#include "metadata/MetadataRecord.h"
#include "tracker/server/TrackerDatabase.h"

namespace tracker { namespace server {

const metadata::MetadataRecord&
TrackerDatabase::findClosest(const std::string& nodeID) {
  return records_[""]; // Placeholder for right now
}

} } // namespace tracker::server
