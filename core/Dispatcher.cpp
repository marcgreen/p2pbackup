
#include "Dispatcher.h"
#include "Job.h"
#include "Worker.h"

namespace core {

Dispatcher::Dispatcher(int numWorkerThreads) :
  numWorkerThreads_(numWorkerThreads),
  nextWorker_(0) {
  workers_.reserve(numWorkerThreads);
  workerThreads_.reserve(numWorkerThreads);
  
  for (int index = 0; index < numWorkerThreads; ++index) {
    workers_.push_back(std::shared_ptr<Worker>(new Worker(index)));
    workerThreads_.push_back(std::thread(Worker::ThreadMain,
					 workers_[index]));
  }
}

Dispatcher::~Dispatcher() {
  this->stop();
}

void Dispatcher::scheduleJob(const Job& job) {
  workers_[nextWorker_]->giveJob(job);
  nextWorker_ = (nextWorker_ + 1) % numWorkerThreads_;
}

void Dispatcher::stop() {
  Job exitJob(JobType::EXIT);
  
  for (std::vector<std::shared_ptr<Worker>>::iterator it = workers_.begin();
       it != workers_.end();
       ++it)
    (*it)->giveJob(exitJob);
  
  for (std::vector<std::thread>::iterator it = workerThreads_.begin();
       it != workerThreads_.end();
       ++it) {
    (*it).join();
  }
}

} // namespace core

