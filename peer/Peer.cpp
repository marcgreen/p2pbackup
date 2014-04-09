
#include "peer/Peer.h"

#include <exception>
#include <sstream>
#include <cstdlib>
#include <memory>
#include <openssl/sha.h> // TODO update f/ heartbleed

namespace peer {
  
  std::shared_ptr<Peer> Peer::instance_ = std::shared_ptr<Peer>(0);
  
  Peer& Peer::constructInstance(std::shared_ptr<metadata::MetadataInterface> metadataI,
				std::shared_ptr<btsync::BTSyncInterface> btSyncI) {
    if (!instance_)
      instance_ = std::shared_ptr<Peer>(new Peer(metadataI, btSyncI));
    return *instance_;
  }

  Peer& Peer::getInstance() {
    return *instance_;
  }

  // private
  Peer::Peer(std::shared_ptr<metadata::MetadataInterface> metadataI, std::shared_ptr<btsync::BTSyncInterface> btSyncI) :
    metadataInterface_(metadataI), btSyncInterface_(btSyncI) { 
}

  bool Peer::joinNetwork() {
    // Calculate our nodeID
    std::string nodeID = Peer::sha256(std::to_string(rand()));
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

    // Find node(s) to back up to
    // left off here
    // loop N times with N random salts, N = # replicas constant
    // keep track of salts (and nodeIDs and fileID) in private data structure
    // create method to serialize ^ to a file
    std::string nodeID = metadataInterface_->findClosestNode(fileID);

    // Determine if node is obligated to store file, given the amount they currently store

    // Ask node to back up this file
    // implement function to do this (develop protocol and implement response function too)

    // Add file to BTSync

    // Add file to metadata layer
  }

  bool Peer::updateFileSize(std::string path, uint64_t) {

  }
  
  std::string Peer::sha256(std::string input) {
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)input.c_str(), input.size(), digest);
    
    std::stringstream output;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
      output << std::hex << (int)digest[i];
    }

    return output.str();
  }

} // namespace peer

main() {
  // TODO find better spot for this?
  srand(0); // srand(time(NULL));

  std::cout << peer::Peer::sha256("test");
}
