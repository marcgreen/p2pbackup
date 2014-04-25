
#ifndef TRACKER_SERVER_TRACKER_H_
#define TRACKER_SERVER_TRACKER_H_

#include <memory>

namespace {

const int TRACKER_POOL_SIZE = 20;

} // namespace

namespace core {

class NetworkController;

} // namespace core

namespace tracker { namespace server {

class Tracker {
 public:
  Tracker();
  void start();
 private:
  std::shared_ptr<core::NetworkController> networkController_;
}; // class Tracker

} } // namespace tracker::server

#endif // TRACKER_SERVER_TRACKER_H
