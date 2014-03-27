
#include "CoreController.h"
#include "Dispatcher.h"

namespace core {

CoreController::CoreController() :
  dispatcher_(std::shared_ptr<Dispatcher>(
                new Dispatcher(DISPATCHER_POOL_SIZE))) {
  
}

void CoreController::submitJob(const Job& job) {
  dispatcher_->scheduleJob(job);
}

} // namespace core
