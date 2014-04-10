
#include "peer/Peer.h"

#include <unistd.h>
#include <stdio.h>
#include <exception>
#include <stdexcept>
#include <sstream>
#include <cstdlib>
#include <memory>
#include <openssl/sha.h> // TODO update f/ heartbleed

namespace peer {
  
  std::shared_ptr<Peer> Peer::instance_ = std::shared_ptr<Peer>(0);
  
  Peer& Peer::constructInstance(std::shared_ptr<metadata::MetadataInterface> metadataI,
				std::shared_ptr<btsync::BTSyncInterface> btSyncI,
				std::string backupDir) {
    if (!instance_)
      instance_ = std::shared_ptr<Peer>(new Peer(metadataI, btSyncI, backupDir));
    return *instance_;
  }

  Peer& Peer::getInstance() {
    return *instance_;
  }

  // private
  Peer::Peer(std::shared_ptr<metadata::MetadataInterface> metadataI,
	     std::shared_ptr<btsync::BTSyncInterface> btSyncI,
	     std::string backupDir) :
    metadataInterface_(metadataI), btSyncInterface_(btSyncI), backupDir_(backupDir) { 
}

  bool Peer::joinNetwork() {
    // Calculate our nodeID
    std::string nodeID = Peer::sha256String(std::to_string(rand()));
    peerID_ = nodeID;

    // Send JOIN command to metadata layer
    try {
      metadataInterface_->joinNetwork(nodeID);
    } catch (std::exception& e) {
      std::cout << e.what() << std::endl;
    }
  }

  bool Peer::blacklistNode(std::string nodeID) {
    metadataInterface_->blacklistNode(peerID_, nodeID);
  }

  bool Peer::backupFile(std::string path) {
    // Calculate sha256 of file contents
    std::string fileID;
    try {
      fileID = sha256File(path);
    } catch (std::exception& e) {
      std::cout << e.what() << std::endl;
      return false;
    }

    // Make directory in our backup directory to house hard link

    // Create hardlink to file in our backup directory
    // int err = link(path, backupDir + "/" + fileID

    // Find node(s) to back up to
    // left off here
    // loop N times with N random salts, N = # replicas constant
    // keep track of salts (and nodeIDs and path and fileID) in private data structure
    // create method to serialize ^ to a file
    std::string nodeID = metadataInterface_->findClosestNode(fileID);

    // Determine if node is obligated to store file, given the amount they currently store

    // Ask node to back up this file
    // implement function to do this (develop protocol and implement response function too)

    // Add file to BTSync

    // Add file to metadata layer
  }

  bool Peer::removeBackup(std::string path) {

  }

  bool Peer::updateFileSize(std::string path, uint64_t) {

  }
  
  std::string Peer::sha256String(std::string input) {
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)input.c_str(), input.size(), digest);
    
    std::stringstream output;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
      output << std::hex << (int)digest[i];
    }

    return output.str();
  }

  std::string Peer::sha256File(std::string path) {
    FILE *f;
    unsigned char buf[8192]; // read 8kb at a time
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256_CTX sc;
    int err;

    f = fopen(path.c_str(), "rb");
    if (f == NULL)
      throw std::runtime_error("Couldn't open file at: " + path);

    if (!SHA256_Init(&sc))
      throw std::runtime_error("Error SHA256 Init");

    for (;;) {
      size_t len = fread(buf, 1, sizeof buf, f);
      if (len == 0)
	break;

      if(!SHA256_Update(&sc, buf, len))
	throw std::runtime_error("Error SHA256 Update");
    }

    err = ferror(f);
    fclose(f);
    if (err)
      throw std::runtime_error("Couldn't close file. Error code: " + err);
    
    if (!SHA256_Final(digest, &sc))
      throw std::runtime_error("Error SHA256 Final");

    std::stringstream output;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
      output << std::hex << (int)digest[i];
    }

    return output.str();
  }

} // namespace peer
