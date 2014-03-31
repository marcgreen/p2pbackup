
#ifndef CORE_BTSYNC_CONTROLLER_H_
#define CORE_BTSYNC_CONTROLLER_H_

#include <memory>

#include "Controller.h"

namespace core {

class Dispatcher;

class BTSyncController : public Controller {
 public:
  BTSyncController(std::shared_ptr<Dispatcher> dispatcher);
  ~BTSyncController();
  void start();
}; // class BTSyncController

} // namespace core

#endif // CORE_BTSYNC_CONTROLLER_H_
