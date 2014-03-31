
#ifndef CORE_NETWORK_CONTROLLER_H_
#define CORE_NETWORK_CONTROLLER_H_

#include <boost/asio.hpp>
#include <memory>

#include "Controller.h"

namespace core {

typedef boost::asio::io_service IOService;
typedef boost::asio::ip::tcp::acceptor TCPAcceptor;
typedef boost::asio::ip::tcp::socket TCPSocket;
typedef boost::asio::ip::tcp::endpoint TCPEndPoint;

class Dispatcher;

class NetworkController : public Controller {
 public:
  NetworkController(std::shared_ptr<Dispatcher> dispatcher);
  ~NetworkController();
  void start();
}; // class NetworkController

} // namespace core

#endif // CORE_NETWORK_CONTROLLER_H_
