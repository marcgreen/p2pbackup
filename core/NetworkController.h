
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

typedef std::function<void(std::shared_ptr<tcp::socket>)>
  NetworkHandlerFunction;

class Dispatcher;

class NetworkController : public Controller {
 public:
  NetworkController(std::shared_ptr<Dispatcher> dispatcher,
		    NetworkHandlerFunction socketHandler);
  ~NetworkController();
  void start();
  void stop();
  void connectionHandler(tcp::socket* socket,
			 const boost::system::error_code& error);
 private:
  boost::asio::io_service ioService_;
  tcp::acceptor acceptor_;
  tcp::endpoint endpoint_;
  bool stopped_;
  std::function<void(std::shared_ptr<tcp::socket>)> socketHandler_;
}; // class NetworkController

void handlePeerSocketConnection(std::shared_ptr<tcp::socket> socket);
void handleTrackerSocketConnection(std::shared_ptr<tcp::socket> socket);

} // namespace core

#endif // CORE_NETWORK_CONTROLLER_H_
