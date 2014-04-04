
#include "core/Dispatcher.h"
#include "core/NetworkController.h"
#include "Tracker.h"

namespace tracker {

Tracker::Tracker() :
  networkController_
  (std::shared_ptr<core::NetworkController>
   (new core::NetworkController
    (std::shared_ptr<core::Dispatcher>
     (new core::Dispatcher
      (TRACKER_POOL_SIZE)),
     core::NetworkHandlerFunction(core::handleTrackerSocketConnection)))) {

}

void Tracker::start() {
  networkController_->start();
}

} // namespace tracker
