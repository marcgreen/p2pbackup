
#ifndef METADATA_INTERFACE_H_
#define METADATA_INTERFACE_H_

#include "MetadataRecord.h"

#include <cstdint>

namespace metadata {

// An interface for interacting with the metadata layer. The implementation
// of the metadata layer can be a centralized tracker, DHT, etc.
// Errors should throw an exception.
class MetadataInterface {
 public:
  // Join the network as a node with the given nodeID
  virtual void joinNetwork(std::string nodeID) = 0;

  // Return the nodeID in the network that most closely matches the given fileID.
  // This is used to find the node to backup data to. fileID could be a hash of the data
  //   being backed up + a salt (to ensure random distribution of replicas)
  virtual std::string findClosestNode(std::string fileID) = 0;
  
  // Fill 'metadataRecord' with the metadata of the node with the given nodeID
  virtual void get(std::string nodeID, MetadataRecord &metadataRecord) = 0;

  // As peerID, blacklist the node with the given nodeID (b/c low uptime, etc)
  // Peers will take into account the number of other peers who've blacklisted a node
  //   during the node selection process.
  virtual void blacklistNode(std::string peerID, std::string nodeID) = 0;

  // Inform the metadata layer that you're backing up 'size' bytes of data identified by
  //   'fileID' to 'nodeID'.
  // This bookkeeping is used during the node selection process to determine if a given node
  //   is obliged to store data (i.e., the node is backing up x bytes of data and is storing
  //   less than k*x bytes, for some implementation specific k)
  virtual void backupFile(std::string nodeID, std::string fileID, uint64_t size) = 0;

  // Inform the metadata layer that the data identified by 'fileID' being backed up
  //   to 'nodeID' is now 'size' bytes. 
  // This will happen when a user backs up a file modifies it in the future. This is
  //   used to reflect deletions, too.
  virtual void updateFileSize(std::string nodeID, std::string fileID, uint64_t size) = 0;

 private:

}; // class MetadataInterface

} // namespace metadata

#endif // METADATA_INTERFACE_H_
