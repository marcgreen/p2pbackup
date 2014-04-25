
#include "core/BTSyncController.h"
#include "core/Dispatcher.h"
#include "peer/Peer.h"

#include <iostream> // For testing purposes only

namespace core {

BTSyncController::BTSyncController(std::shared_ptr<Dispatcher> dispatcher,
				   bool deferChecking) :
  Controller(dispatcher), shouldStop_(false), deferChecking_(deferChecking) {
  
}

BTSyncController::~BTSyncController() {
  
}

void BTSyncController::start() {
  peer::Peer& peer = peer::Peer::getInstance();
  if (deferChecking_) {
    for (int round = 0; round < 60 && !shouldStop_; ++round)
      std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  while (!shouldStop_) {
    // Look at the file size for each backed up file.
    // Compare it to the LocalBackupInfo data structure in Peer
    // If there are any differences between the two, update the LocalBackupInfo
    // data structure and push the changes to the Metadata Layer.
    peer.synchronizeWithBTSync();
    
    // A bit of a hack, but it will do for now.
    for (int round = 0; round < 60 && !shouldStop_; ++round)
      std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

void BTSyncController::stop() {
  shouldStop_ = true;
}

} // namespace core
