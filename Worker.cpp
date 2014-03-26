
#include "Worker.h"

#include <iostream> // For testing purposes only

namespace core {

Worker::Worker(const int id, const Dispatcher *master) :
  id_(id),
  master_(master) {
  
}

void Worker::startJob(const Job& job) {
  currentJob_ = job;
  wakeup_cond_.notify_one();
}

void Worker::ThreadMain(Worker *worker) {
  std::unique_lock<std::mutex> ulock(worker->wakeup_mutex_);
  bool done = false;
  
  while (!done) {
    worker->wakeup_cond_.wait(ulock);
    
    switch (worker->currentJob_.type) {
    case EXECUTE:
      worker->currentJob_.func();
      break;
    case EXIT:
      done = true;
      break;
    case NOP:
      break;
    default:
      throw "Unknown job type";
    }
  }
}

} // namespace core
