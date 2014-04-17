
#include "core/MetadataController.h"
#include "peer/Peer.h"

#include <chrono>

namespace core {

MetadataController::MetadataController
  (std::shared_ptr<Dispatcher> dispatcher) :
    Controller(dispatcher), shouldStop_(false) {
  
}

MetadataController::~MetadataController() {
  
}

void MetadataController::start() {
  peer::Peer& peer = peer::Peer::getInstance();
  while (!shouldStop_) {
    peer.checkOnBackupNodes();
    peer.checkMetadataForStoreChanges();
    // A bit of a hack, but it will do for now.
    for (int round = 0; round < 5 && !shouldStop_; ++round)
      std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

void MetadataController::stop() {
  shouldStop_ = true;
}

} // namespace core
