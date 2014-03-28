
#ifndef CORE_CORE_CONTROLLER_H_
#define CORE_CORE_CONTROLLER_H_

#include <memory>
#include <vector>

namespace {

const int DISPATCHER_POOL_SIZE = 10;

} // namespace

namespace core {

class Dispatcher;
class Job;
class Controller;

class CoreController {
 public:
  void registerController(const std::shared_ptr<Controller>& Controller);
 private:
  std::vector<std::shared_ptr<Controller>> registeredControllers_;
}; // class CoreController

} // namespace core

#endif // CORE_CORE_CONTROLLER_H_
