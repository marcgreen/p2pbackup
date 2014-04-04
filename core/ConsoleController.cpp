
#include "BTSyncController.h"
#include "ConsoleController.h"
#include "Controller.h"
#include "Dispatcher.h"
#include "NetworkController.h"

#include <chrono>
#include <iostream>

namespace core {

ConsoleController::ConsoleController() :
  Controller(std::shared_ptr<Dispatcher>
	     (new Dispatcher(DISPATCHER_POOL_SIZE))) {
  createControllers();
}

ConsoleController::ConsoleController(std::shared_ptr<Dispatcher> dispatcher) :
  Controller(dispatcher) {
  createControllers();
}

void ConsoleController::start() {
  startAllAsync();
  std::string input;
  std::cin >> input;
  stopAllAsync();
}

void ConsoleController::createControllers() {
  asyncControllers_.push_back
    (std::shared_ptr<Controller>
     (new NetworkController
      (dispatcher_, NetworkHandlerFunction(handlePeerSocketConnection))));
  asyncControllers_.push_back(std::shared_ptr<Controller>
			      (new BTSyncController(dispatcher_)));
}

void ConsoleController::startAllAsync() {
  for (std::vector<std::shared_ptr<Controller>>::iterator it =
	 asyncControllers_.begin();
       it != asyncControllers_.end();
       ++it)
    asyncControllerThreads_.push_back((*it)->startInBackground());
}

void ConsoleController::stopAllAsync() {
  for (std::vector<std::shared_ptr<Controller>>::iterator it =
	 asyncControllers_.begin();
	 it != asyncControllers_.end();
	 ++it) {
    (*it)->stop();
  }
  
  // Use a second loop so that the shutdown signal can be given to all threads
  // before trying to join any of them.
  for (std::vector<std::shared_ptr<std::thread>>::iterator it =
	 asyncControllerThreads_.begin();
	 it != asyncControllerThreads_.end();
	 ++it) {
    (*it)->join();
  }
}

} // namespace core
