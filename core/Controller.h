
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
  void sendShutdownSignal();
  std::shared_ptr<std::thread> startInBackground();
  unsigned long id() { return id_; }
 protected:
  std::shared_ptr<Dispatcher> dispatcher_;
  bool shouldStop_;
 private:
  unsigned long id_;
  static unsigned long nextID;
}; // class Controller

} // namespace core

#endif // CORE_CONTROLLER_H_
