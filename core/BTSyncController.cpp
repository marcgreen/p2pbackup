
#include "core/BTSyncController.h"
#include "core/Dispatcher.h"
#include "peer/Peer.h"

#include <iostream> // For testing purposes only

namespace core {

BTSyncController::BTSyncController(std::shared_ptr<Dispatcher> dispatcher) :
  Controller(dispatcher), shouldStop_(false) {
  
}

BTSyncController::~BTSyncController() {
  
}

void BTSyncController::start() {
  peer::Peer& peer = peer::Peer::getInstance();
  while (!shouldStop_) {
    // Look at the file size for each backed up file.
    // Compare it to the LocalBackupInfo data structure in Peer
    // If there are any differences between the two, update the LocalBackupInfo
    // data structure and push the changes to the Metadata Layer.
    peer.synchronizeWithBTSync();
    
    // Have the thread go to sleep so that it can check for updates in
    // a little bit.
    std::this_thread::sleep_for(std::chrono::seconds(60));
  }
}

void BTSyncController::stop() {
  shouldStop_ = true;
}

} // namespace core
