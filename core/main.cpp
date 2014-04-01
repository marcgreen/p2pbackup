
#include <chrono>
#include <thread>

#include <iostream>

#include "Dispatcher.h"
#include "Job.h"

//#include "ConsoleController.h"

void fib(unsigned long n) {
  unsigned long total = 1;
  for (unsigned long x = 1; x <= n; ++x)
    total *= x;
  //std::cout << "fib(" << n << ") = " << total << std::endl;
}

int main(int argc, char **argv) {
  
  core::Dispatcher d(10);
  
  std::chrono::time_point<std::chrono::system_clock> start =
    std::chrono::system_clock::now();
  for (int y = 0; y < 1000; ++y)
    for (int x = 0; x <= 10; ++x)
      d.scheduleJob(core::Job(std::bind(&fib, x)));
  std::chrono::time_point<std::chrono::system_clock> end =
    std::chrono::system_clock::now();
  int elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>
    (end - start).count();
  std::cout << "elapsed_time = " << elapsed_time << std::endl;
  
  return 0;
}
