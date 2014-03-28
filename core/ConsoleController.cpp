
#include "ConsoleController.h"

namespace core {

ConsoleController::ConsoleController(std::shared_ptr<Dispatcher> dispatcher) :
  Controller(dispatcher) {
  // TODO: add the other Controllers (network and BTSync updater) to the
  // asyncControllers_ vector here.
}

void ConsoleController::start() {
  
}

} // namespace core
