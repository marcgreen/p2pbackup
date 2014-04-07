
#include "Dispatcher.h"
#include "Job.h"
#include "NetworkController.h"

#include <boost/bind.hpp>

#include <functional>
#include <iostream> // For testing purposes only

namespace core {

NetworkController::NetworkController(std::shared_ptr<Dispatcher> dispatcher,
				     NetworkHandlerFunction socketHandler) :
  Controller(dispatcher),
  acceptor_(ioService_),
  endpoint_(tcp::v4(), CONTROLLER_PORT),
  stopped_(false),
  socketHandler_(socketHandler) {
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
    Job networkJob(std::bind(socketHandler_, socket_ptr));
    dispatcher_->scheduleJob(networkJob);
  }
}

} // namespace core
