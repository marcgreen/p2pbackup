
#include <chrono>
#include <thread>

#include "core/ConsoleController.h"

int main(int argc, char **argv) {
  core::ConsoleController controller;
  controller.start();
  
  return 0;
}