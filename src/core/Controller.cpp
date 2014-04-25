
#include "Controller.h"

#include <thread>

namespace core {

Controller::Controller(std::shared_ptr<Dispatcher> dispatcher) :
  dispatcher_(dispatcher) {
  
}

Controller::~Controller() { }

std::shared_ptr<std::thread> Controller::startInBackground() {
  std::shared_ptr<std::thread> backgroundThread =
    std::shared_ptr<std::thread>(new std::thread(&Controller::start, this));
  return backgroundThread;
}

} // namespace core
