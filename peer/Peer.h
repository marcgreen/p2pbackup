
#ifndef PEER_PEER_H_
#define PEER_PEER_H_

#include "btsync/BTSyncInterface.h"
#include "metadata/MetadataInterface.h"
#include "metadata/LocalBackupInfo.h"

#include <memory>
#include <mutex>

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
  // - Update metadata
  bool backupFile(std::string path);

  // Store the file with the given encryption secret for a node
  bool storeFile(std::string secret);

  // Stop backing up the file at path
  // - Remove file from BTSync
  // - Update metadata
  // - Delete hardlink
  // - Replicant node's metadata controller will notice metadata update and remove replica
  bool removeBackup(std::string fileID);

  // Update the metadata layer with a file's new size
  bool updateFileSize(std::string fileID, uint64_t size);
  
  // Give potential replicant node at nodeIP the secret to store.
  // Return whether or not they ACK
  bool askNodeToBackup(std::string nodeIP, std::string secret);

  // Create the given directory if it's not already created
  void createDirIfNeeded(std::string path);
  
  // Update the LocalBackupInfo and the Metadata Layer with BitTorrent Sync.
  void synchronizeWithBTSync();
  
  // Return the SHA256 digest for input
  static std::string sha256String(std::string input);

  // Return the SHA256 digest for the file at path
  // Reads 8kb of file at a time
  // Throws runtime error if things go wrong
  static std::string sha256File(std::string path);

  // Return a salt to be concat'd to contents of pre-hash data
  static std::string salt();

  const int DEFAULT_BTSYNC_PORT = 11589;
  const float MAX_BLACKLIST_STORE_RATIO = .25;
  const int TOTAL_REPLICA_COUNT = 5;
  const std::string BACKUP_DIR = "backup";
  const std::string STORE_DIR = "store";
  const std::string LOCAL_BACKUP_INFO_FILE = "local_backup_info";
 private:
  // Create necessary directories for backing up and storing data
  // Read in localBackupInfo from disk
  Peer(std::shared_ptr<metadata::MetadataInterface> metadataI,
       std::shared_ptr<btsync::BTSyncInterface> btSyncI,
       std::string backupDir);

  static std::shared_ptr<Peer> instance_;

  // Keep track of our own ID
  std::string peerID_;

  // The folder in which we will place hardlinks of backed up files and store other peers' files
  // Should have two subdirectories: BACKUP_DIR and STORE_DIR
  std::string btBackupDir_;

  std::shared_ptr<btsync::BTSyncInterface> btSyncInterface_;
  std::shared_ptr<metadata::MetadataInterface> metadataInterface_;

  // Data structure used to keep track of all files we're backing up.
  // This is persisent; it's written to/read from disk.
  // BTSyncController uses it for synchronizing filesizes between BTSync and Metadata Layer
  // MetadataController uses it to keep track of how often a node fails a challenge, etc
  metadata::LocalBackupInfo localBackupInfo_;
  
  // Mutex to protect access to localBackupInfo.
  // TODO: update all uses of localBackupInfo_ so that they use this mutex
  std::recursive_mutex localInfoMutex_;

}; // class Peer

} // namespace peer

#endif // PEER_PEER_H_
