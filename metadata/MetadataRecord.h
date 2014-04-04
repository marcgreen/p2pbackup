
#ifndef METADATA_RECORD_H_
#define METADATA_RECORD_H_

#include <string>

namespace metadata {

// The actual metadata
class MetadataRecord {
 public:
  MetadataRecord(std::string ip);
  std::string toString();

  const int NODE_ID_BYTES = 20;
  const int FILE_ID_BYTES = 20;
  const int FILE_SIZE_BYTES = 8;

 private:
  std::string nodeIP_;
  // TODO rest of metadata record

}; // class MetadataRecord

} // namespace metadata

#endif // METADATA_RECORD_H_
