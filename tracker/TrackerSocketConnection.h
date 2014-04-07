
#ifndef TRACKER_TRACKER_SOCKET_CONNECTION_H_
#define TRACKER_TRACKER_SOCKET_CONNECTION_H_

#include <boost/asio.hpp>
#include <memory>

namespace tracker {

using boost::asio::ip::tcp;

void handleTrackerSocketConnection(std::shared_ptr<tcp::socket> socket);

} // namespace tracker

#endif // TRACKER_TRACKER_SOCKET_CONNECTION_H_
