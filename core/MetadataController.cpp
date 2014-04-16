
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
    std::this_thread::sleep_for(std::chrono::seconds(60));
  }
}

void MetadataController::stop() {

}

} // namespace core
