
#ifndef CORE_NETWORK_CONTROLLER_H_
#define CORE_NETWORK_CONTROLLER_H_

#include <boost/asio.hpp>
#include <memory>

#include "Controller.h"

namespace {

int CONTROLLER_PORT = 57000;

} // namespace

namespace core {

using boost::asio::ip::tcp;

class Dispatcher;

class NetworkController : public Controller {
 public:
  NetworkController(std::shared_ptr<Dispatcher> dispatcher);
  ~NetworkController();
  void start();
  void stop();
  void connectionHandler(tcp::socket* socket,
			 const boost::system::error_code& error);
  void handleSocketConnection(std::shared_ptr<tcp::socket> socket);
 private:
  boost::asio::io_service ioService_;
  tcp::acceptor acceptor_;
  tcp::endpoint endpoint_;
  bool stopped_;
}; // class NetworkController

} // namespace core

#endif // CORE_NETWORK_CONTROLLER_H_
