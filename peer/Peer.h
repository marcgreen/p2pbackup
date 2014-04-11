
#ifndef PEER_PEER_H_
#define PEER_PEER_H_

#include "btsync/BTSyncInterface.h"
#include "metadata/MetadataInterface.h"
#include "metadata/LocalBackupInfo.h"

#include <memory>

namespace peer {

// Abstraction of what a "peer" is in our system. Used as glue between 
//   the core system and BTSyncInterface/TrackerInterface.
// Implemented as a Singleton.
class Peer {
 public:
  // We need the two interfaces and the directory where we'll create hardlinks to the backup files
  static Peer& constructInstance(std::shared_ptr<metadata::MetadataInterface> metadataI,
				 std::shared_ptr<btsync::BTSyncInterface> btSyncI,
				 std::string backupDir);
  static Peer& getInstance();
  
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

  // Store the file with the given encryption secret for a node
  bool storeFile(std::string secret);

  // Stop backing up the file at path
  // - Remove file from BTSync
  // - Update tracker
  bool removeBackup(std::string path);

  // Update the metadata layer with a file's new size
  bool updateFileSize(std::string fileID, uint64_t size);
  
  // Give potential replicant node at nodeIP the secret to store.
  // Return whether or not they ACK
  bool askNodeToBackup(std::string nodeIP, std::string secret);

  // Return the SHA256 digest for input
  static std::string sha256String(std::string input);

  // Return the SHA256 digest for the file at path
  // Reads 8kb of file at a time
  // Throws runtime error if things go wrong
  static std::string sha256File(std::string path);

  // Return a salt to be concat'd to contents of pre-hash data
  static std::string salt();

  const float MAX_BLACKLIST_STORE_RATIO = .25;
  const int TOTAL_REPLICA_COUNT = 5;
  const std::string BACKUP_DIR = "backup";
  const std::string STORE_DIR = "store";

 private:
  Peer(std::shared_ptr<metadata::MetadataInterface> metadataI,
       std::shared_ptr<btsync::BTSyncInterface> btSyncI,
       std::string backupDir);

  static std::shared_ptr<Peer> instance_;

  // Keep track of our own ID
  std::string peerID_;

  // Keep track of salts used to find replication nodes.
  // pathToHardLink => (originalPath, fileID, salt, nodeID, size)
  // TODO how to deal with moved files? may need new command for client, or maybe we can auto detect
  // std::map<std::string, >;
  // TODO create dumpToDisk for this 
  // use JSON for data structure
  // make public getter with const reference so metadata controller can access
  // The folder we will place hardlinks of backed up files and store other peers' files
  // Should have two subdirectories: BACKUP_DIR and STORE_DIR
  // TODO create those dirs if not present
  std::string btBackupDir_;

  std::shared_ptr<btsync::BTSyncInterface> btSyncInterface_;
  std::shared_ptr<metadata::MetadataInterface> metadataInterface_;
	metadata::LocalBackupInfo localbackupInfo_;
}; // class Peer

} // namespace peer

#endif // PEER_PEER_H_
