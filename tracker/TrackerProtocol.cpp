
#include "tracker/TrackerProtocol.h"

#include <boost/asio.hpp>
#include <cstdint>
#include <jsoncpp/writer.h>
#include <memory>

namespace tracker {

bool recv(Json::Value& dataToReceive, tcp::socket& socket) {
  uint32_t messageSize;
  bool result;
  
  // boost::asio::read can throw a system error on failure
  try {
    // Read the size of the JSON to come
    boost::asio::read(socket,
		      boost::asio::buffer(&messageSize, sizeof(messageSize)));
    
    std::cout << "Size of message read as " << messageSize << std::endl;
		
    // Allocate enough space for the JSON
    std::unique_ptr<char> jsonData(new char[messageSize]);
    
    // Read the JSON
    boost::asio::read(socket, boost::asio::buffer(jsonData.get(), messageSize));
    
    // Convert the JSON to its Json::Value form
    Json::Reader converter;
    result = converter.parse(jsonData.get(), dataToReceive);
  } catch(boost::system::system_error& error) {
    result = false;
  }
  
  return result;
}

bool send(const Json::Value& dataToSend, tcp::socket& socket) {
  Json::FastWriter serializer;
  bool result;
  std::string serializedData = serializer.write(dataToSend);
  const char *cData = serializedData.data();
  uint32_t serializedSize = serializedData.length() + 1;
  
  try {
    // Send the size of the serialized JSON
    boost::asio::write(socket, boost::asio::buffer(&serializedSize,
						   sizeof(serializedSize)));
    
    // Send the serialized JSON
    boost::asio::write(socket, boost::asio::buffer(cData, serializedSize));
    
    // Everything was sent successfully
    result = true;
  } catch(boost::system::system_error& error) {
    result = false;
  }
  
  return result;
}

} // namespace tracker
