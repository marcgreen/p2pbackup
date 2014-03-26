
#include "Dispatcher.h"

#include <chrono>

int main(int argc, char **argv) {
  core::Dispatcher d(10);
  std::this_thread::sleep_for(std::chrono::seconds(2));
  
  d.testWakeup();
  
  return 0;
}
