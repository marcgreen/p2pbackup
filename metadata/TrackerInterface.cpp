#include "TrackerInterface.h"
#include "MetadataRecord.h"

#include <string>
#include <iostream>

namespace metadata {

TrackerInterface::TrackerInterface(std::string ip, std::string port) :
  serverIP_(ip), serverPort_(port) {
    
}

  MetadataRecord TrackerInterface::get(std::string key) {
    std::cout << "Getting value for key '" << key << "'" << std::endl;
    
    MetadataRecord value = MetadataRecord("10.10.10.10");
    // Connect to server, retrieve, and return

    return value;
  }

  void TrackerInterface::put(std::string key, MetadataRecord value) {
    std::cout << "Inserting key/value pair:" << std::endl
	      << key << "/" << value.toString() << std::endl;

    // Connect to server and put data
  }

} // namespace metadata

int main(int argc, char *argv[]) {
  using namespace std;

  string example_key = "39dk3KNJDF9832N";
  metadata::MetadataRecord example_record = metadata::MetadataRecord("2.20.2.20");
  metadata::TrackerInterface tr = metadata::TrackerInterface("127.0.0.1", "6262");

  tr.put(example_key, example_record);
  cout << tr.get(example_key).toString() << endl;
}
