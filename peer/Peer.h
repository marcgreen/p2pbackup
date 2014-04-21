
#ifndef PEER_PEER_H_
#define PEER_PEER_H_

#include "btsync/BTSyncInterface.h"
#include "metadata/MetadataInterface.h"
#include "metadata/LocalBackupInfo.h"

#include <boost/asio.hpp>
#include <memory>
#include <mutex>

namespace peer {

using boost::asio::ip::tcp;

// Abstraction of what a "peer" is in our system. Used as glue between 
//   the core system and BTSyncInterface/TrackerInterface.
// Implemented as a Singleton.
class Peer {
 public:
  // We need the two interfaces and the directory where we'll create hardlinks to the backup files
  static Peer& constructInstance(std::shared_ptr<metadata::MetadataInterface> metadataI,
				 std::shared_ptr<btsync::BTSyncInterface> btSyncI,
				 std::string backupDir,
				 std::string localBackupInfoLocation = std::string());
  
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
  
  // Creates a single replica of the file with ID fileID.
  bool createReplica(const std::string& fileID,
		     const std::string& rwSecret,
		     const std::string& encryptionSecret,
		     uint64_t filesize);

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
  
  // Unregisters nodeID as a backup for the file with read/write secret
  // rwSecret.
  bool unregisterBackupNode(const std::string& nodeID,
			    const std::string& rwSecret);
  
  // Create the given directory if it's not already created
  void createDirIfNeeded(std::string path);
  
  // Synchronize the metadata layer (and our localBackupInfo) with the correct size
  //   of all files, as determined by BTSync
  void synchronizeWithBTSync();
  
  // Check on all nodes that this peer uses to backup, and move any backups if
  // necessary. This is called by the MetadataController
  void checkOnBackupNodes();
  
  // Check to see if any peers have removed you from their backup nodes.
  void checkMetadataForStoreChanges();
  
  static const int ENCRYPTION_SECRET_LENGTH;
  static const int DEFAULT_BTSYNC_PORT;
  static const std::string DEFAULT_BTSYNC_PORT_STR;
  static const float MAX_BLACKLIST_STORE_RATIO;
  static const int TOTAL_REPLICA_COUNT; // TODO change when testing large scale
  static const std::string BACKUP_DIR;
  static const std::string STORE_DIR;
  static const std::string LOCAL_BACKUP_INFO_FILE;
  static const int BTSYNC_FOLDER_RESCAN_INTERVAL;
  static const int METADATA_RESCAN_INTERVAL;
  static const uint64_t MINIMUM_STORE_SIZE;
  static const uint32_t STARTING_NODE_RELIABILITY;
 private:
  // Create necessary directories for backing up and storing data
  // Read in localBackupInfo from disk
  Peer(std::shared_ptr<metadata::MetadataInterface> metadataI,
       std::shared_ptr<btsync::BTSyncInterface> btSyncI,
       std::string backupDir,
       std::string localBackupInfoLocation);
  
  void recreateAndRegisterBTSyncDirectories();
  
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
  std::recursive_mutex localInfoMutex_;

}; // class Peer

} // namespace peer

#endif // PEER_PEER_H_
