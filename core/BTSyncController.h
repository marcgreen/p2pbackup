
#ifndef CORE_BTSYNC_CONTROLLER_H_
#define CORE_BTSYNC_CONTROLLER_H_

#include <chrono>
#include <memory>
#include <thread>

#include "Controller.h"

namespace core {

class Dispatcher;

class BTSyncController : public Controller {
 public:
  BTSyncController(std::shared_ptr<Dispatcher> dispatcher,
		   bool deferChecking_ = false);
  ~BTSyncController();
  void start();
  void startInBackground();
  void stop();
 private:
  bool shouldStop_;
  bool deferChecking_;
}; // class BTSyncController

} // namespace core

#endif // CORE_BTSYNC_CONTROLLER_H_
