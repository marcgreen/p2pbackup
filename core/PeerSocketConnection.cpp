
#include "core/PeerSocketConnection.h"
#include "peer/Peer.h"

#include <boost/asio.hpp>

namespace core {

void handlePeerSocketConnection(std::shared_ptr<tcp::socket> socket) {
	char secretBuffer[20]; // Secrets are 20 bytes long
  
	try {
		// Get the 20 byte secret from the network
		boost::asio::read(*socket, boost::asio::buffer(secretBuffer, 20));
		uint8_t ack = 1;
		boost::asio::write(*socket, boost::asio::buffer(&ack, sizeof(ack)));
	} catch(boost::system::system_error& error) {
		std::cerr << "Failure in message exchage between peers. "
							<< "Error = " << error.what() << std::endl;
	}
	
	std::string secret(secretBuffer, 20);
	peer::Peer& peer = peer::Peer::getInstance();
	bool storeSuccessful = peer.storeFile(secret);
	
	if (storeSuccessful)
		std::cout << "Successful registered file to store "
							<< "with encryption secret " << secret << std::endl;
	else
		std::cerr << "Registering file store with encryption secret "
							<< secret << " was unsuccessful" << std::endl;
}

} // namespace core
