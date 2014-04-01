
#include "Dispatcher.h"
#include "Job.h"
#include "NetworkController.h"

#include <functional>
#include <iostream> // For testing purposes only

namespace core {

NetworkController::NetworkController(std::shared_ptr<Dispatcher> dispatcher) :
  Controller(dispatcher) {

}

NetworkController::~NetworkController() {
  std::cout << "NetworkController shutting down" << std::endl;
}

void NetworkController::start() {
  boost::asio::io_service ioService;
  boost::system::error_code ec;
  tcp::acceptor acceptor(ioService, tcp::endpoint(tcp::v4(), CONTROLLER_PORT));
  
  while (true) {
    std::shared_ptr<tcp::socket> nextConnection =
      std::shared_ptr<tcp::socket>(new tcp::socket(ioService));
    acceptor.accept(*nextConnection, ec);
    
    if (!ec) {
      Job networkJob(std::bind(&NetworkController::handleSocketConnection,
			       this, nextConnection));
    }
  }
}

void NetworkController::stop() {
  
}

void NetworkController::mainListenerLoop() {
  
}

void NetworkController::handleSocketConnection(
  std::shared_ptr<tcp::socket> socket) {
  
}

} // namespace core
