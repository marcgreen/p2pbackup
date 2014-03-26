
#include "Dispatcher.h"
#include "Job.h"
#include "Worker.h"

#include <iostream> // For testing purposes only

namespace core {

void Dispatcher::testWakeup() {
  
}

Dispatcher::Dispatcher(int numWorkerThreads) {
  workers_.reserve(numWorkerThreads);
  workerThreads_.reserve(numWorkerThreads);
  
  for (int index = 0; index < numWorkerThreads; ++index) {
    available_.push(index);
    workers_.push_back(new Worker(index, this));
    workerThreads_.push_back(std::thread(Worker::ThreadMain,
					 workers_[index]));
  }
}

Dispatcher::~Dispatcher() {
  // TODO: acquire available mutex, set the global done flag (make this), for
  // each waiting worker signal them with the EXIT job.
    
  for (std::vector<std::thread>::iterator it = workerThreads_.begin();
       it != workerThreads_.end();
       ++it) {
    (*it).join();
  }
  
  for(std::vector<Worker*>::iterator it = workers_.begin();
      it != workers_.end();
      ++it) {
    delete (*it);
  }
}

void Dispatcher::freeWorker(int workerID) {
  available_mutex_.lock();
  available_.push(workerID);
  available_mutex_.unlock();
}

bool Dispatcher::scheduleJob(const Job& job) {
  available_mutex_.lock();
  if (available_.empty()) {
    available_mutex_.unlock();
    return false;
  }
  int chosenWorker = available_.front();
  available_.pop();
  workers_[chosenWorker]->startJob(job);
  available_mutex_.unlock();
  return true;
}

} // namespace core

