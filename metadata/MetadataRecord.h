
#ifndef METADATA_METADATA_RECORD_H_
#define METADATA_METADATA_RECORD_H_

#include <list>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace metadata {

// Metadata on an individual file being backed up
// Used to determine total size of data backed up from and stored on a target node
class FileMetadata {
 public:
  FileMetadata() {}
  FileMetadata(std::string nodeID_, uint64_t size_, time_t timestamp_) :
   nodeID(nodeID_), size(size_), timestamp(timestamp_) {}

  // The node the file belongs to
  std::string nodeID;

  // Size of file in bytes
  uint64_t size;

  // timestamp when file was most recently backed up
  time_t timestamp;
};

// The actual metadata stored in the DHT
class MetadataRecord {
 public:
  MetadataRecord();
  MetadataRecord(std::string ip);

  void setNodeIP(std::string nodeIP) { 
    nodeIP_ = nodeIP;
  }

  std::string getNodeIP() {
    return nodeIP_;
  }

  int getNumberBlacklisters() {
    return blacklisters_.size();
  }

  // Adds a blacklister to the list. Duplicate IDs are ignored.
  // Returns whether or not the blacklister was added
  bool addBlacklister(std::string peerID, time_t timestamp);

  // Return the sum of all filesizes this peer is backing up
  uint64_t getTotalBackupSize();

  // Return the number of files this node is storing.
  // Used to calculate blacklister:store ratio, which is used to determine if this node is "good"
  int getNumberStoredFiles() {
    return blacklisters_.size();
  }

  // Take note of a file being backed up to nodeID with the given size
  bool addBackupFile(std::string fileID, std::string nodeID, uint64_t size);

  // Update the size of a file already being backed up
  // If size is 0, this will delete all records of the file
  bool updateBackupFileSize(std::string fileID, uint64_t size);

  // Return the sum of all filesizes this peer is storing
  uint64_t getTotalStoreSize();

  // Take note of a file being stored on this node by peerID with the given size
  bool addStoreFile(std::string fileID, std::string peerID, uint64_t size);

  // Update the size of a file already being stored
  // If size is 0, this will delete all records of the file
  // Returns whether the update was successful
  bool updateStoreFileSize(std::string fileID, uint64_t size);

  // Serialize this class into JSON
  // Note: this function will prune the blacklisters as a side effect (for efficiency)
  std::string serialize();

  // Unserialize the reply and fill out this data structure
  bool unserialize(std::string reply);

  std::string toString() {
    // TODO use serialize and json::value to return pretty printed string
    return nodeIP_;
  }
	
  std::list<FileMetadata>::iterator backupNodeIteratorBegin
    (const std::string& fileID);
	
  std::list<FileMetadata>::iterator backupNodeIteratorEnd
    (const std::string& fileID);

 private:
  // IP address of the node this record describes
  std::string nodeIP_;

  // Map of <nodeID, timestamp> pairs to keep track of peers who blacklist this node.
  // Map is used to quickly check for duplicate IDs. Duplicate entries are ignored.
  std::unordered_map<std::string, time_t> blacklisters_;

  // Keep track of filesizes this peer is backing up to nodes. Key is fileID
  std::map<std::string, std::list<FileMetadata>> backedUpFiles_;

  // Keep track of filesizes this node is backing up for other peers. Key is fileID
  std::map<std::string, FileMetadata> storedFiles_;

  // keep blacklisters for 1 week and then prune them to give the node another chance
  const time_t PRUNE_AGE = 7 * 24 * 60 * 60;

}; // class MetadataRecord

} // namespace metadata

#endif // METADATA_METADATA_RECORD_H_
