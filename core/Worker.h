
#ifndef CORE_WORKER_H_
#define CORE_WORKER_H_

#include <chrono>
#include <condition_variable>
#include <memory>
#include <thread>

#include <iostream>

namespace core {

class Job;
class Dispatcher;

// Contains information on a single worker thread.
// The thread object itself is not stored in this object because a pointer to
// this object is passed to the static ThreadMain function, which is the
// entry point for the threads. If the thread object was in this object, this
// object would be passed to the thread before initialization is done.
class Worker {
 public:
  //Worker(const int id);
  Worker(Dispatcher *dispatcher);
  void giveJob(const Job& job);
  
  static void ThreadMain(std::shared_ptr<Worker> worker);
 private:
  //const int id_;
  Dispatcher *dispatcher_;
  std::queue<Job> jobQueue_;
  std::mutex jobQueueMutex_;
  
  constexpr static const std::chrono::milliseconds timeout =
    std::chrono::milliseconds(1);
  
  Job getNextJob();
  bool tryForNextJob(Job& job);
}; // class Worker

} // namespace core

#endif // CORE_WORKER_H_
