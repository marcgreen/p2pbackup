#include "BTSync.h"

#include <sstream>
#include <cstdlib>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

#include <jsoncpp/json.h>

namespace btsync {

BTSync::BTSync(std::string username, std::string password, std::string ip, std::string port) :
  api_url_("http://" + username + ":" + password + "@" + ip + ":" + port + "/api?method=") {
  // left off determining which curl/json variables should be global to the class, 
  //   then implement methods using main() as example
}

Json::Value getFolders() {

}

Json::Value getFolders(std::string secret) {}
Json::Value addFolder(std::string path, bool selective_sync = false) {}
Json::Value addFolder(std::string path, std::string secret, bool selective_sync = false) {}
Json::Value removeFolder(std::string secret) {}
Json::Value getFiles(std::string secret) {}
Json::Value getFiles(std::string secret, std::string path) {}
Json::Value setFilePreferences(std::string secret, std::string path, bool download) {}
Json::Value getFolderPeers(std::string secret) {}
Json::Value getSecrets(bool encrypted = false) {}
Json::Value getSecrets(std::string secret, bool encrypted = false) {}
Json::Value getFolderPreferences(std::string secret) {}
Json::Value setFolderPreferences(std::string secret, Json::Value params) {}
Json::Value getFolderHosts(std::string secret) {}
Json::Value setFolderHosts(std::string secret, Json::Value hosts) {}
Json::Value getPreferences() {}
Json::Value setPreferences(Json::Value params) {}
Json::Value getOSName() {}
Json::Value getVersion() {}
Json::Value getSpeed() {}
Json::Value shutdown() {}

} // namespace btsync

int main(int argc, char *argv[]) {
  using namespace std;
  string url = "http://mqp:btsync@127.0.0.1:8888/api?method=get_folders";
  stringstream json;

  try {
    curlpp::Cleanup cleaner;
    curlpp::Easy request;

    json << curlpp::options::Url(url);

    cout << url << endl;
    cout << json.str() << endl;
  }
  catch ( curlpp::LogicError & e ) {
    cout << e.what() << endl;
  }
  catch ( curlpp::RuntimeError & e ) {
    cout << e.what() << endl;
  }

  Json::Value root;
  Json::Reader reader;
  bool parsedSuccess = reader.parse(json.str(), root);

  if (!parsedSuccess) {
    cout << "Failed to parse json" << endl;
    return 1;
  }

  Json::Value folder = root[(unsigned int) 0];

  string dir = folder.get("dir", "defaultvalue").asString();
  cout << "dir: " << dir << endl;

  string secret = folder["secret"].asString();
  cout << "secret: " << secret << endl;

  root[(unsigned int) 0]["test"] = 3;
  cout << "Json pretty print" << endl
       << root.toStyledString() << endl;

}
