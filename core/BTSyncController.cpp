
#include "BTSyncController.h"
#include "Dispatcher.h"

#include <iostream> // For testing purposes only

namespace core {

BTSyncController::BTSyncController(std::shared_ptr<Dispatcher> dispatcher) :
  Controller(dispatcher) {
  
}

BTSyncController::~BTSyncController() {
  std::cout << "BTSyncController shutting down" << std::endl;
}

void BTSyncController::start() {
  while (!shouldStop_)
    std::cout << "BTSyncController" << std::endl;
}

} // namespace core
