
#include "Dispatcher.h"
#include "NetworkController.h"

#include <iostream> // For testing purposes only

namespace core {

NetworkController::NetworkController(std::shared_ptr<Dispatcher> dispatcher) :
  Controller(dispatcher) {

}

NetworkController::~NetworkController() {
  std::cout << "NetworkController shutting down" << std::endl;
}

void NetworkController::start() {
  using boost::asio::ip::tcp;
  
  boost::asio::io_service ioService;
  tcp::acceptor acceptor(ioService, tcp::endpoint(tcp::v4(), CONTROLLER_PORT));
  
  while (!shouldStop_) {
    tcp::socket nextConnection(ioService);
    acceptor.accept(nextConnection);
    
  }
}

} // namespace core
