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

}

Json::Value BTSync::request_(std::string url_params) {
  std::stringstream json;
  std::string url = api_url_ + url_params;

  try {
    curlpp::Cleanup cleaner;
    curlpp::Easy request;

    json << curlpp::options::Url(url);
  }
  catch ( curlpp::LogicError & e ) {
    std::cout << e.what() << std::endl;
    return Json::Value(false);
  }
  catch ( curlpp::RuntimeError & e ) {
    std::cout << e.what() << std::endl;
    return Json::Value(false);
  }

  Json::Value root;
  Json::Reader reader;
  bool parsedSuccess = reader.parse(json.str(), root);

  if (!parsedSuccess) {
    std::cout << "Failed to parse json: " << json.str() << std::endl
	      << "Url requested: " << url << std::endl;
    return Json::Value(false);
  }

  // DEBUG
  std::cout << "Url: " + url << std::endl;

  return root;
}

Json::Value BTSync::getFolders() {
  return request_("get_folders");
}

Json::Value BTSync::getFolders(std::string secret) {
  return request_("get_folders&secret=" + secret);
}

Json::Value BTSync::addFolder(std::string path, bool selective_sync) {
  std::cout << "2arg addfolder" << std::endl;
  return request_("add_folder&dir=" + path + 
		  "&selective_sync=" + std::string(selective_sync ? "1" : "0"));
}

Json::Value BTSync::addFolder(std::string path, std::string secret, bool selective_sync) {
  std::cout << "3arg addfolder" << std::endl;
  return request_("add_folder&dir=" + path + 
		  "&secret=" + std::string(secret) +
		  "&selective_sync=" + std::string(selective_sync ? "1" : "0"));
}

Json::Value BTSync::addFolder(std::string path, const char *secret, bool selective_sync) {
  return BTSync::addFolder(path, std::string(secret), selective_sync);
}

Json::Value BTSync::removeFolder(std::string secret) {
  return request_("remove_folder&secret=" + secret);
}

Json::Value BTSync::getFiles(std::string secret) {
  return request_("get_files&secret=" + secret);
}

Json::Value BTSync::getFiles(std::string secret, std::string path) {
  return request_("get_files&secret=" + secret +
		  "&path=" + path);
}

Json::Value BTSync::setFilePreferences(std::string secret, std::string path, bool download) {
  return request_("set_file_prefs&secret=" + secret +
		  "&path=" + path +
		  "&download=" + std::string(download ? "1" : "0"));
}

  // left off testing here in main

Json::Value BTSync::getFolderPeers(std::string secret) {
  return request_("get_folder_peers&secret=" + secret);
}

Json::Value BTSync::getSecrets(bool encrypted) {
  return request_("get_secrets" + std::string(encrypted ? "&type=encryption" : ""));
}

  Json::Value BTSync::getSecrets(std::string secret, bool encrypted) {
  return request_("get_secrets" + std::string(encrypted ? "&type=encryption" : "") +
		  "&secret=" + std::string(secret));
}

Json::Value BTSync::getSecrets(const char *secret, bool encrypted) {
  return BTSync::getSecrets(std::string(secret), encrypted); 
}

Json::Value BTSync::getFolderPreferences(std::string secret) {
  return request_("get_folder_prefs&secret=" + secret);
}

Json::Value BTSync::setFolderPreferences(std::string secret, Json::Value params) {
  std::string prefs;
  std::vector<std::string> members = params.getMemberNames();

  // format JSON params as "&param1=value1&param2=value2...."
  for (auto &it : members) {
    prefs += "&" + it + "=" + params[it].asString();
  }

  return request_("set_folder_prefs&secret=" + secret + prefs);
}

Json::Value BTSync::getFolderHosts(std::string secret) {
  return request_("get_folder_hosts&secret=" + secret);
}

Json::Value BTSync::setFolderHosts(std::string secret, Json::Value hosts) {
  std::string csv;
  for (unsigned int i = 0; i < hosts.size(); i++) {
    if (i != 0) csv += ",";
    csv += hosts[i].asString();
  }
  return request_("set_folder_hosts&secret=" + secret +
		  "&hosts=" + csv);
}

Json::Value BTSync::getPreferences() {}
Json::Value BTSync::setPreferences(Json::Value params) {}
Json::Value BTSync::getOSName() {}
Json::Value BTSync::getVersion() {}
Json::Value BTSync::getSpeed() {}
Json::Value BTSync::shutdown() {}

} // namespace btsync

int main(int argc, char *argv[]) {
  using namespace std;

  btsync::BTSync bts = btsync::BTSync("mqp", "btsync", "127.0.0.1", "8888");
  string secret = "DJCEQ5N3SQVBI4THJNEN2HUDZPDJJZSNF";

  /*
  Json::Value root = bts.getFolders();
  Json::Value folder = root[(unsigned int) 0];

  string secret = folder["secret"].asString();
  cout << "secret: " << secret << endl;

  cout << "Json pretty print" << endl
       << root.toStyledString() << endl;

  cout << bts.addFolder("/home/mqp/Documents").toStyledString() << endl;
  cout << bts.addFolder("/home/mqp", true).toStyledString() << endl;
  cout << bts.addFolder("/home/mqp/Downloads", "A4FOUMFWROQXOS2JIFMM6GQGQATVC5BWU").toStyledString() << endl;
  cout << bts.addFolder("/home/mqp/p2pbackup/btsync", "A5FOUMFWROQXOS2JIFMM6GQGQATVC5BWU", true).toStyledString() << endl;
  cout << bts.removeFolder("A5FOUMFWROQXOS2JIFMM6GQGQATVC5BWU").toStyledString() << endl;

  cout << bts.getFiles("DJCEQ5N3SQVBI4THJNEN2HUDZPDJJZSNF").toStyledString() << endl;
  cout << bts.getFiles("DJCEQ5N3SQVBI4THJNEN2HUDZPDJJZSNF", "subfolder").toStyledString() << endl;

  cout << bts.setFilePreferences(secret, "threadtest.cpp", true).toStyledString() << endl;
  cout << bts.setFilePreferences(secret, "threadtest.cpp", false).toStyledString() << endl;
  */
  
  // left off see results of these tests
  cout << bts.getFolderPeers(secret).toStyledString() << endl;

  cout << bts.getSecrets().toStyledString() << endl;
  cout << bts.getSecrets(secret).toStyledString() << endl;
  cout << bts.getSecrets(secret, true).toStyledString() << endl;

  cout << bts.getFolderPreferences(secret).toStyledString() << endl;

  Json::Value folderPrefs;
  folderPrefs["use_hosts"] = 1;
  folderPrefs["use_dht"] = 0;
  cout << bts.setFolderPreferences(secret, folderPrefs).toStyledString() << endl;

  cout << bts.getFolderHosts(secret).toStyledString() << endl;

  Json::Value folderHosts;
  folderHosts[(unsigned int) 0] = "192.168.1.1:4567";
  folderHosts[(unsigned int) 1] = "10.10.10.10:1010";
  cout << bts.setFolderHosts(secret, folderHosts).toStyledString() << endl;
}
