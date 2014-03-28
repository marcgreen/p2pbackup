
#include "CoreController.h"
#include "Dispatcher.h"

namespace core {

void CoreController::registerController(const std::shared_ptr
				      <Controller>& controller) {
  registeredControllers_.push_back(controller);
}

} // namespace core
