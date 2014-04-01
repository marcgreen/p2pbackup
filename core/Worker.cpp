
#include "Dispatcher.h"
#include "Job.h"
#include "Worker.h"

namespace core {

  /*Worker::Worker(const int id) :
  id_(id) {
  
  }*/

Worker::Worker(Dispatcher *dispatcher) :
  dispatcher_(dispatcher) {

}

void Worker::giveJob(const Job& job) {
  jobQueueMutex_.lock();
  jobQueue_.push(job);
  jobQueueMutex_.unlock();
}

Job Worker::getNextJob() {
  Job nextJob;
  
  while (true) {
    if (tryForNextJob(nextJob))
      return nextJob;
    else
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}

bool Worker::tryForNextJob(Job& job) {
  bool retrievedJob = false;
  jobQueueMutex_.lock();
  if (!jobQueue_.empty()) {
    retrievedJob = true;
    job = jobQueue_.front();
    jobQueue_.pop();
  }
  jobQueueMutex_.unlock();
  return retrievedJob;
}

// static
void Worker::ThreadMain(std::shared_ptr<Worker> worker) {
  while (worker->dispatcher_->doJob());
}

} // namespace core
