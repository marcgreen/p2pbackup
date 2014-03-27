
#ifndef CORE_JOB_H_
#define CORE_JOB_H_

#include <functional>

namespace core {

typedef std::function<void()> JobFunction;

enum JobType {
  EXECUTE, // Execute the provided function
  EXIT, // Don't execute the provided function, exit the thread
  NOP // Don't execute the provided function, don't exit the thread
}; // enum JobType

class Job {
 public:
  Job(JobType type_ = NOP, JobFunction func_ = JobFunction());
  
  JobFunction func;
  JobType type;
}; // class Job

} // namespace core

#endif // CORE_JOB_H_
