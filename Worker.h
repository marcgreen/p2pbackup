
#ifndef WORKER_H_
#define WORKER_H_

#include "Job.h"

#include <condition_variable>
#include <thread>

namespace core {

class Dispatcher;

// Contains information on a single worker thread.
// The thread object itself is not stored in this object because a pointer to
// this object is passed to the static ThreadMain function, which is the
// entry point for the threads. If the thread object was in this object, this
// object would be passed to the thread before initialization is done.
class Worker {
 public:
  Worker(const int id, const Dispatcher *master);
  void startJob(const Job& job);
  
  static void ThreadMain(Worker *worker);
 private:
  const int id_;
  const Dispatcher *master_;
  std::condition_variable wakeup_cond_;
  std::mutex wakeup_mutex_;
  Job currentJob_;
};

} // namespace core

#endif // WORKER_H_
