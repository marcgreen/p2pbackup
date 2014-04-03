#include "BTSyncInterface.h"

#include <sstream>
#include <cstdlib>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

#include <jsoncpp/json.h>

namespace btsync {

BTSyncInterface::BTSyncInterface(std::string username, std::string password, 
				 std::string ip, std::string port) :
  api_url_("http://" + username + ":" + password + "@" + 
	   ip + ":" + port + "/api?method=") {

}

Json::Value BTSyncInterface::getFolders() {
  return request_("get_folders");
}

Json::Value BTSyncInterface::getFolders(std::string secret) {
  return request_("get_folders&secret=" + secret);
}

Json::Value BTSyncInterface::addFolder(std::string path, bool selective_sync) {
  return request_("add_folder&dir=" + path + 
		  "&selective_sync=" + std::string(selective_sync ? "1" : "0"));
}

Json::Value BTSyncInterface::addFolder(std::string path, std::string secret, 
				       bool selective_sync) {
  return request_("add_folder&dir=" + path + 
		  "&secret=" + std::string(secret) +
		  "&selective_sync=" + std::string(selective_sync ? "1" : "0"));
}

Json::Value BTSyncInterface::addFolder(std::string path, const char *secret, 
				       bool selective_sync) {
  return BTSyncInterface::addFolder(path, std::string(secret), selective_sync);
}

Json::Value BTSyncInterface::removeFolder(std::string secret) {
  return request_("remove_folder&secret=" + secret);
}

Json::Value BTSyncInterface::getFiles(std::string secret) {
  return request_("get_files&secret=" + secret);
}

Json::Value BTSyncInterface::getFiles(std::string secret, std::string path) {
  return request_("get_files&secret=" + secret +
		  "&path=" + path);
}

Json::Value BTSyncInterface::setFilePreferences(std::string secret,
						std::string path,
						bool download) {
  return request_("set_file_prefs&secret=" + secret +
		  "&path=" + path +
		  "&download=" + std::string(download ? "1" : "0"));
}

Json::Value BTSyncInterface::getFolderPeers(std::string secret) {
  return request_("get_folder_peers&secret=" + secret);
}

Json::Value BTSyncInterface::getSecrets(bool encrypted) {
  return request_("get_secrets" + 
		  std::string(encrypted ? "&type=encryption" : ""));
}

  Json::Value BTSyncInterface::getSecrets(std::string secret, bool encrypted) {
  return request_("get_secrets" + 
		  std::string(encrypted ? "&type=encryption" : "") +
		  "&secret=" + std::string(secret));
}

Json::Value BTSyncInterface::getSecrets(const char *secret, bool encrypted) {
  return BTSyncInterface::getSecrets(std::string(secret), encrypted); 
}

Json::Value BTSyncInterface::getFolderPreferences(std::string secret) {
  return request_("get_folder_prefs&secret=" + secret);
}

Json::Value BTSyncInterface::setFolderPreferences(std::string secret, 
						  Json::Value params) {
  std::string prefs;
  std::vector<std::string> members = params.getMemberNames();
  
  // format JSON params as "&param1=value1&param2=value2...."
  for (std::string it : members) {
    prefs += "&" + it + "=" + jsonValueToString_(params[it]);
  }
  
  return request_("set_folder_prefs&secret=" + secret + prefs);
}

Json::Value BTSyncInterface::getFolderHosts(std::string secret) {
  return request_("get_folder_hosts&secret=" + secret);
}

Json::Value BTSyncInterface::setFolderHosts(std::string secret, 
					    Json::Value hosts) {
  std::string csv;
  for (unsigned int i = 0; i < hosts.size(); i++) {
    if (i != 0) csv += ",";
    csv += hosts[i].asString();
  }

  return request_("set_folder_hosts&secret=" + secret +
		  "&hosts=" + csv);
}

Json::Value BTSyncInterface::getPreferences() {
  return request_("get_prefs");
}

Json::Value BTSyncInterface::setPreferences(Json::Value params) {
  std::string prefs;
  std::vector<std::string> members = params.getMemberNames();
  
  // format JSON params as "&param1=value1&param2=value2...."
  for (std::string it : members) {
    prefs += "&" + it + "=" + jsonValueToString_(params[it]);
  }
  
  return request_("set_prefs" + prefs);
}

Json::Value BTSyncInterface::getOSName() {
  return request_("get_os");
}

Json::Value BTSyncInterface::getVersion() {
  return request_("get_version");
}

Json::Value BTSyncInterface::getSpeed() {
  return request_("get_speed");
}

Json::Value BTSyncInterface::shutdown() {
  return request_("shutdown");
}

Json::Value BTSyncInterface::request_(std::string url_params) {
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

std::string BTSyncInterface::jsonValueToString_(Json::Value jsonValue) {
  std::string stringValue;

  // May need to check for more types in the future
  if (jsonValue.type() == Json::ValueType::intValue)
    stringValue = std::to_string(jsonValue.asInt());
  else 
    stringValue = jsonValue.asString();

  return stringValue;
}

} // namespace btsync

int main(int argc, char *argv[]) {
  using namespace std;

  btsync::BTSyncInterface bts = 
    btsync::BTSyncInterface("mqp", "btsync", "127.0.0.1", "8888");
  string secret = "DJCEQ5N3SQVBI4THJNEN2HUDZPDJJZSNF";

  Json::Value root = bts.getFolders();
  Json::Value folder = root[(unsigned int) 0];

  string sec = folder["secret"].asString();
  cout << "secret: " << sec << endl;

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

  cout << bts.getFolderPeers(secret).toStyledString() << endl;

  cout << bts.getSecrets().toStyledString() << endl;
  cout << bts.getSecrets(true).toStyledString() << endl;
  cout << bts.getSecrets(secret).toStyledString() << endl;
  cout << bts.getSecrets(secret, true).toStyledString() << endl;
  cout << bts.getSecrets(secret, false).toStyledString() << endl;
  //TODO test getSecrets more once we understand how it is supposed to work

  Json::Value folderPrefs = bts.getFolderPreferences(secret);
  cout << folderPrefs.toStyledString() << endl;

  Json::Value newPrefs;
  newPrefs["use_hosts"] = 1 - folderPrefs["use_hosts"].asInt();
  newPrefs["use_dht"] = 1 - folderPrefs["use_dht"].asInt();
  cout << bts.setFolderPreferences(secret, newPrefs).toStyledString() << endl;

  cout << bts.getFolderHosts(secret).toStyledString() << endl;

  Json::Value folderHosts;
  cout << bts.setFolderHosts(secret, folderHosts).toStyledString() << endl;

  folderHosts[(unsigned int) 0] = "192.168.1.1:4567";
  folderHosts[(unsigned int) 1] = "10.10.10.10:1010";
  cout << bts.setFolderHosts(secret, folderHosts).toStyledString() << endl;

  Json::Value prefs = bts.getPreferences();
  cout << prefs.toStyledString() << endl;

  prefs["device_name"] = prefs["device_name"].asString() + "_";
  cout << bts.setPreferences(prefs).toStyledString() << endl;

  cout << bts.getOSName().toStyledString() << endl;
  cout << bts.getVersion().toStyledString() << endl;
  cout << bts.getSpeed().toStyledString() << endl;
  cout << bts.shutdown().toStyledString() << endl;
  
}
 
