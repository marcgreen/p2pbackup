
#ifndef CORE_DISPATCHER_H_
#define CORE_DISPATCHER_H_

#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace {

const int DISPATCHER_POOL_SIZE = 10;

} // namespace
namespace core {

class Job;
class Worker;

class Dispatcher {
 public:
  Dispatcher(int numWorkerThreads);
  ~Dispatcher();
  void scheduleJob(const Job& job);
  void stop();
 private:
  std::vector<std::shared_ptr<Worker>> workers_;
  std::vector<std::thread> workerThreads_;
  const int numWorkerThreads_;
  int nextWorker_;
}; // class Dispatcher

} // namespace core

#endif // CORE_DISPATCHER_H_
