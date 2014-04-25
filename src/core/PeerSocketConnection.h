
#ifndef CORE_PEER_SOCKET_CONNECTION_H_
#define CORE_PEER_SOCKET_CONNECTION_H_

#include <boost/asio.hpp>
#include <memory>

namespace core {

using boost::asio::ip::tcp;

void handlePeerSocketConnection(std::shared_ptr<tcp::socket> socket);

} // namespace core

#endif // CORE_PEER_SOCKET_CONNECTION_H_
