
#ifndef TRACKER_PROTOCOL_H_
#define TRACKER_PROTOCOL_H_

#include <boost/asio.hpp>
#include <jsoncpp/json.h>

namespace tracker {

using boost::asio::ip::tcp;

// Enum to indicate which command is being sent
enum TrackerCommand {
	JOIN_NETWORK_CMD = 0, FIND_CLOSEST_NODE_CMD, GET_CMD, 
	BLACKLIST_NODE_CMD, BACKUP_FILE_CMD, UPDATE_FILE_SIZE_CMD
};

// Used to receive a message from the network.
bool recv(Json::Value& dataToReceive, tcp::socket& socket);
// Used to send a message to the network.
bool send(const Json::Value& dataToSend, tcp::socket& socket);

} // namespace tracker

#endif // TRACKER_PROTOCOL_H_
