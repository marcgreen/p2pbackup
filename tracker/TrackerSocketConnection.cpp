
#include "tracker/TrackerProtocol.h"
#include "tracker/TrackerSocketConnection.h"

#include <cstdint>
#include <iostream>
#include <vector>

namespace tracker {

void handleTrackerSocketConnection(std::shared_ptr<tcp::socket> socket) {
  Json::Value value;
  recv(value, *socket);
}

} // namespace tracker
