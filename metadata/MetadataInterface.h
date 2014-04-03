
#ifndef METADATA_INTERFACE_H_
#define METADATA_INTERFACE_H_

#include "MetadataRecord.h"

namespace metadata {

// An interface for interacting with the metadata layer. The implementation
// of the metadata layer can be a centralized tracker, DHT, etc.
class MetadataInterface {
 public:
  virtual MetadataRecord get(std::string key) = 0;
  virtual void put(std::string key, MetadataRecord value) = 0;

 private:

}; // class MetadataInterface

} // namespace metadata

#endif // METADATA_INTERFACE_H_
