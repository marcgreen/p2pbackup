
#ifndef METADATA_RECORD_H_
#define METADATA_RECORD_H_

#include <string>

namespace metadata {

// The actual metadata
class MetadataRecord {
 public:
  MetadataRecord() {}
  MetadataRecord(std::string ip);

  void setNodeIP(std::string nodeIP) { 
    nodeIP_ = nodeIP;
  }

  std::string getNodeIP() {
    return nodeIP_;
  }

  std::string toString() {
    return nodeIP_;
  }

  // Not sure if we need these constants
  const int NODE_ID_BYTES = 20;
  const int FILE_ID_BYTES = 20;
  const int FILE_SIZE_BYTES = 8;

 private:
  std::string nodeIP_;
  // TODO rest of metadata record

}; // class MetadataRecord

} // namespace metadata

#endif // METADATA_RECORD_H_
