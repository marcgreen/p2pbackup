
#ifndef PEER_PEER_H_
#define PEER_PEER_H_

#include "btsync/BTSyncInterface.h"
#include "metadata/MetadataInterface.h"

#include <memory>

namespace peer {

// Abstraction of what a "peer" is in our system. Used as glue between 
//   the core system and BTSyncInterface/TrackerInterface.
// Implemented as a Singleton.
class Peer {
 public:
  Peer& constructInstance(std::shared_ptr<metadata::MetadataInterface> metadataI,
			  std::shared_ptr<btsync::BTSyncInterface> btSyncI);
  Peer& getInstance();
  
  // Contact metadata layer to join the P2P network
  bool joinNetwork();

  // Blacklist a node.
  // Used when a node storing our replica is not adequately available
  // In the future, will be used as part of a thorough challenege mechanism
  bool blacklistNode(std::string nodeID);

  // Backup the file located at "path"
  // - Find a node to backup to
  // - Add file to BTSync
  // - Update tracker
  bool backupFile(std::string path);

  // Stop backing up the file at path
  // - Remove file from BTSync
  // - Update tracker
  bool removeBackup(std::string path);

  // Update the metadata layer with a file's new size
  bool updateFileSize(std::string path, uint64_t size);
  
  // Return the SHA256 digest for input
  static std::string sha256String(std::string input);

  // Return the SHA256 digest for the file at path
  // Reads 8kb of file at a time
  // Throws runtime error if things go wrong
  static std::string sha256File(std::string path);

 private:
  Peer(std::shared_ptr<metadata::MetadataInterface> metadataI, std::shared_ptr<btsync::BTSyncInterface> btSyncI);

  static std::shared_ptr<Peer> instance_;

  // Keep track of our own ID
  std::string peerID_;

  std::shared_ptr<btsync::BTSyncInterface> btSyncInterface_;
  std::shared_ptr<metadata::MetadataInterface> metadataInterface_;
}; // class Peer

} // namespace peer

#endif // PEER_PEER_H_
