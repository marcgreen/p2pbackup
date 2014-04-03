
#ifndef METADATA_RECORD_H_
#define METADATA_RECORD_H_

#include <string>

namespace metadata {

// This class is the actual metadata
class MetadataRecord {
 public:
  MetadataRecord(std::string ip);
  std::string toString();

 private:
  std::string nodeIP_;
  // TODO rest of metadata record

}; // class MetadataRecord

} // namespace metadata

#endif // METADATA_RECORD_H_
