
#ifndef CORE_CONSOLE_CONTROLLER_H_
#define CORE_CONSOLE_CONTROLLER_H_

#include <memory>
#include <vector>
#include <thread>

#include "Controller.h"

namespace core {

class CoreController;

class ConsoleController : public Controller {
 public:
  ConsoleController(std::shared_ptr<Dispatcher> dispatcher);
  void start();
 private:
  std::vector<std::shared_ptr<Controller>> asyncControllers_;
  std::vector<std::shared_ptr<std::thread>> asyncControllerThreads_;
}; // class ConsoleController

} // namespace core

#endif // CORE_CONSOLE_CONTROLLER_H_
