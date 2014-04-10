
#ifndef TRACKER_SERVER_TRACKER_SOCKET_CONNECTION_H_
#define TRACKER_SERVER_TRACKER_SOCKET_CONNECTION_H_

#include "tracker/server/TrackerDatabase.h"

#include <boost/asio.hpp>
#include <jsoncpp/json.h>
#include <memory>

namespace tracker { namespace server {

using boost::asio::ip::tcp;

void handleTrackerSocketConnection(std::shared_ptr<tcp::socket> socket);

// Helper Functions
void handleJoin(std::shared_ptr<tcp::socket> socket,
		Json::Value& networkData,
		TrackerDatabase& trackerDatabase);
void handleFindClosestNode(std::shared_ptr<tcp::socket> socket,
			   Json::Value& networkData,
			   TrackerDatabase& trackerDatabase);
void handleGet(std::shared_ptr<tcp::socket> socket,
	       Json::Value& networkData,
	       TrackerDatabase& trackerDatabase);
void handleBlacklist(std::shared_ptr<tcp::socket> socket,
		     Json::Value& networkData,
		     TrackerDatabase& trackerDatabase);
void handleBackup(std::shared_ptr<tcp::socket> socket,
		  Json::Value& networkData,
		  TrackerDatabase& trackerDatabase);
void handleUpdateFileSize(std::shared_ptr<tcp::socket> socket,
			  Json::Value& networkData,
			  TrackerDatabase& trackerDatabase);

} } // namespace tracker::server

#endif // TRACKER_SERVER_TRACKER_SOCKET_CONNECTION_H_
