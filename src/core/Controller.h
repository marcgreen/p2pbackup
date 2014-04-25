
#ifndef CORE_CONTROLLER_H_
#define CORE_CONTROLLER_H_

#include <memory>
#include <thread>

namespace core {

class Dispatcher;

class Controller {
 public:
  Controller(std::shared_ptr<Dispatcher> dispatcher);
  virtual ~Controller();
  virtual void start() = 0;
  std::shared_ptr<std::thread> startInBackground();
  virtual void stop() = 0;
 protected:
  std::shared_ptr<Dispatcher> dispatcher_;
}; // class Controller

} // namespace core

#endif // CORE_CONTROLLER_H_
