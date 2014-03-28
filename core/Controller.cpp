
#include "Controller.h"

#include <thread>

namespace core {

unsigned long Controller::nextID = 0;

Controller::Controller(std::shared_ptr<Dispatcher> dispatcher) :
  dispatcher_(dispatcher), id_(nextID), shouldStop_(false) {
  ++nextID;
}

Controller::~Controller() { }

void Controller::sendShutdownSignal() {
  shouldStop_ = true;
}

std::shared_ptr<std::thread> Controller::startInBackground() {
  std::shared_ptr<std::thread> backgroundThread =
    std::shared_ptr<std::thread>(new std::thread(&Controller::start, this));
  return backgroundThread;
}

} // namespace core
