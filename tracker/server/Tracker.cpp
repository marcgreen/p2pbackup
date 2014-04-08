
#include "core/Dispatcher.h"
#include "core/NetworkController.h"
#include "Tracker.h"
#include "TrackerSocketConnection.h"

namespace tracker { namespace server {

Tracker::Tracker() :
  networkController_
  (std::shared_ptr<core::NetworkController>
   (new core::NetworkController
    (std::shared_ptr<core::Dispatcher>
     (new core::Dispatcher
      (TRACKER_POOL_SIZE)),
     core::NetworkHandlerFunction
     (tracker::server::handleTrackerSocketConnection)))) {

}

void Tracker::start() {
  networkController_->start();
}

} } // namespace tracker::server
