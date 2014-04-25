
#include "tracker/server/Tracker.h"

#include <cstdlib>
#include <ctime>

int main(int argc, char **argv) {
  srand(std::time(NULL));
  tracker::server::Tracker t;
  t.start();
  return 0;
}
