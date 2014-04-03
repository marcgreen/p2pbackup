#include <string>

#include "MetadataRecord.h"

namespace metadata {

  MetadataRecord::MetadataRecord(std::string ip) :
    nodeIP_(ip) {

  }

  std::string MetadataRecord::toString() {
    return nodeIP_;
  }

} // namespace metadata
