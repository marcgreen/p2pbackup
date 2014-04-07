
#ifndef TRACKER_PROTOCOL_H_
#define TRACKER_PROTOCOL_H_

namespace tracker {

// Enum to indicate which command is being sent
enum TrackerCommand = { JOIN_NETWORK_CMD = 0, FIND_CLOSEST_NODE_CMD, GET_CMD, 
			BLACKLIST_NODE_CMD, BACKUP_FILE_CMD, UPDATE_FILE_SIZE_CMD };

} // namespace tracker

#endif // TRACKER_PROTOCOL_H_
