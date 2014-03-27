
#ifndef CORE_CORE_CONTROLLER_H_
#define CORE_CORE_CONTROLLER_H_

#include <memory>

namespace {

const int DISPATCHER_POOL_SIZE = 10;

} // namespace

namespace core {

class Dispatcher;
class Job;

class CoreController {
 public:
  CoreController();
  void submitJob(const Job& job);
 private:
  std::shared_ptr<Dispatcher> dispatcher_;

}; // class CoreController

} // namespace core

#endif // CORE_CORE_CONTROLLER_H_
