
#include "Dispatcher.h"
#include "Job.h"
#include "NetworkController.h"

#include <boost/bind.hpp>

#include <functional>
#include <iostream> // For testing purposes only

namespace core {

NetworkController::NetworkController(std::shared_ptr<Dispatcher> dispatcher) :
  Controller(dispatcher),
  acceptor_(ioService_),
  endpoint_(tcp::v4(), CONTROLLER_PORT),
  stopped_(false) {
  acceptor_.open(endpoint_.protocol());
  acceptor_.set_option(tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint_);
  acceptor_.listen();
}

NetworkController::~NetworkController() {
  std::cout << "NetworkController shutting down" << std::endl;
  stop();
}

void NetworkController::start() {
  // TODO:
  // 1. Create the io_service object in the constructor of the
  // NetworkController.
  // 2. Create the acceptor object in the constructor of the
  // NetworkController (?).
  // 3. In this method, have an infinite loop that (1) creates a new socket
  // object, (2) uses the acceptor to call async_accept, and (3) call run so
  // that the handler is run on this thread.
  // 4. In the handler, pass the socket off to a worker thread in the
  // dispatcher.
  // 5. After the handler returns, execution should return to this thread so
  // that another connection can be handled.
  // 6. In the stop() method of this class, cancel() will be called, which will
  // stop the asynchronous wait for new connections.
  boost::system::error_code ec;
  
  while (!stopped_) {
    tcp::socket *nextConnection = new tcp::socket(ioService_);
    acceptor_.async_accept(*nextConnection,
			   boost::bind(&NetworkController::connectionHandler,
				       this, nextConnection,
				       boost::asio::placeholders::error));
    ioService_.run();
    ioService_.reset();
  }
}

void NetworkController::stop() {
  if (!stopped_) {
    acceptor_.close();
    stopped_ = true;
  }
}

void NetworkController::connectionHandler(
  tcp::socket* socket,
  const boost::system::error_code& error) {
  std::shared_ptr<tcp::socket> socket_ptr(socket);
  if (!error) {
    Job networkJob(std::bind(&NetworkController::handleSocketConnection,
			     this, socket_ptr));
    dispatcher_->scheduleJob(networkJob);
  }
}

void NetworkController::handleSocketConnection(
  std::shared_ptr<tcp::socket> socket) {
  std::cout << "Hey" << std::endl;
}

} // namespace core
