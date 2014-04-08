
#ifndef TRACKER_SERVER_TRACKER_SOCKET_CONNECTION_H_
#define TRACKER_SERVER_TRACKER_SOCKET_CONNECTION_H_

#include <boost/asio.hpp>
#include <memory>

namespace tracker { namespace server {

using boost::asio::ip::tcp;

void handleTrackerSocketConnection(std::shared_ptr<tcp::socket> socket);

} } // namespace tracker::server

#endif // TRACKER_SERVER_TRACKER_SOCKET_CONNECTION_H_
