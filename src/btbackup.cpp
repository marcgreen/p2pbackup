
#include "core/ConsoleController.h"

int main(int argc, char **argv) {
  core::ConsoleController controller;
  
  if (argc > 1) {
    std::string localBackupInfoLocation(argv[1]);
    controller.start(localBackupInfoLocation);
  } else {
    controller.start();
  }
  
  return 0;
}
