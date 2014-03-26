
#ifndef DISPATCHER_H_
#define DISPATCHER_H_

#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace core {

class Job;
class Worker;

class Dispatcher {
 public:
  Dispatcher(int numWorkerThreads);
  ~Dispatcher();
  void freeWorker(int workerID);
  bool scheduleJob(const Job& job);
  void testWakeup();
 private:
  std::vector<Worker*> workers_;
  std::vector<std::thread> workerThreads_;
  std::queue<int> available_;
  std::mutex available_mutex_;
}; // class Dispatcher

} // namespace core

#endif // DISPATCHER_H_
