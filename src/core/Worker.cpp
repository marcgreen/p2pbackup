
#include "Dispatcher.h"
#include "Job.h"
#include "Worker.h"

namespace core {

Worker::Worker(Dispatcher *dispatcher) :
  dispatcher_(dispatcher) {

}

void Worker::threadMain() {
  while (dispatcher_->doJob());
}

} // namespace core
