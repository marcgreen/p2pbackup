
#ifndef BTSYNC_INTERFACE_H_
#define BTSYNC_INTERFACE_H_

#include <jsoncpp/json.h>

namespace btsync {

// Interface to BTSync API
// Most functions exactly map to the BTSync API documentation. Exceptions are documented inline.
// Errors will cause fuctions to return Json::Value(false).
class BTSyncInterface {
 public:
  BTSyncInterface(std::string username, std::string password, std::string ip, std::string port);
  Json::Value getFolders();
  Json::Value getFolders(std::string secret);
  Json::Value addFolder(std::string path, bool selective_sync = false);
  Json::Value addFolder(std::string path, std::string secret, bool selective_sync = false);
  Json::Value addFolder(std::string path, const char *secret, bool selective_sync = false);

  Json::Value removeFolder(std::string secret);
  Json::Value getFiles(std::string secret);
  Json::Value getFiles(std::string secret, std::string path);
  Json::Value setFilePreferences(std::string secret, std::string path, bool download);
  Json::Value getFolderPeers(std::string secret);

  // The BTSync API docs say 'secret' is a required parameter, but it isn't.
  // The BTSync API implements the 'encrypted' parameter as a string; it's a bool here
  Json::Value getSecrets(bool encrypted = false);
  Json::Value getSecrets(std::string secret, bool encrypted = false);
  Json::Value getSecrets(const char *secret, bool encrypted = false);

  Json::Value getFolderPreferences(std::string secret);

  // The 'params' parameter should look like: { "use_dht": 0, "use_hosts": 1, ... }
  Json::Value setFolderPreferences(std::string secret, Json::Value params);

  Json::Value getFolderHosts(std::string secret);

  // The 'hosts' param should look like: { "hosts": [ "host1:port1", "host2:port2", ... ] }
  Json::Value setFolderHosts(std::string secret, Json::Value hosts);

  Json::Value getPreferences();

  // The 'params' parameter should look like: { "device_name": "iMac", "use_upnp": 0, ... }
  Json::Value setPreferences(Json::Value params);

  Json::Value getOSName();
  Json::Value getVersion();
  Json::Value getSpeed();
  Json::Value shutdown();
 private:
  std::string api_url_;

  // Return the JSON from the BTSync http request with the given parameters
  Json::Value request_(std::string url_params);

  // This should be used when the Json::Value might not be a string
  std::string jsonValueToString_(Json::Value jsonValue);

}; // class BTSyncInterface

} // namespace btsync

#endif // BTSYNC_INTERFACE_H_
