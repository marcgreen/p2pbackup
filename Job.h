
#ifndef JOB_H_
#define JOB_H_

#include <functional>

namespace core {

typedef std::function<void()> JobFunction;

enum JobType {
  EXECUTE, // Execute the provided function
  EXIT, // Don't execute the provided function, exit the thread
  NOP // Don't execute the provided function, don't exit the thread
};

class Job {
 public:
  Job(JobFunction func_ = JobFunction(), JobType type_ = NOP);
  
  JobFunction func;
  JobType type;
};

} // namespace core

#endif // JOB_H_
