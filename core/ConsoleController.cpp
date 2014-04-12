
#include "core/BTSyncController.h"
#include "core/ConsoleController.h"
#include "core/Controller.h"
#include "core/Dispatcher.h"
#include "core/NetworkController.h"
#include "core/PeerSocketConnection.h"
#include "metadata/MetadataInterface.h"
#include "peer/Peer.h"
#include "tracker/client/TrackerInterface.h"

#include <boost/algorithm/string.hpp>
#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

namespace core {

ConsoleController::ConsoleController() :
  Controller(std::shared_ptr<Dispatcher>
	     (new Dispatcher(DISPATCHER_POOL_SIZE))) {
  createControllers();
}

ConsoleController::ConsoleController(std::shared_ptr<Dispatcher> dispatcher) :
  Controller(dispatcher) {
  createControllers();
}

// TODO: for each command, call the appropriate Peer method
void ConsoleController::start() {
  bool exitCommandUsed = false;
  Json::Value configInfo;
  bool configSuccessful = getTrackerInfo(configInfo);
  
  if (!configSuccessful) {
    std::cerr << "Error: no valid configuration file found." << std::endl
	      << "Please create a file called config.json next to "
	      << "this executable to use this program." << std::endl
	      << "See the documentation for this program for more "
	      << "information about what should go in the "
	      << "configuration file." << std::endl;
    return;
  }
  
  peer::Peer& peer = peer::Peer::constructInstance
    (std::shared_ptr<metadata::MetadataInterface>
     (new tracker::client::TrackerInterface
      (configInfo["TrackerIP"].asString(),
       configInfo["TrackerPort"].asString())),
     std::shared_ptr<btsync::BTSyncInterface>
     (new btsync::BTSyncInterface
      (configInfo["BTSyncUsername"].asString(),
       configInfo["BTSyncPassword"].asString(),
       configInfo["BTSyncIP"].asString(),
       configInfo["BTSyncPort"].asString())),
     configInfo["BackupDirectory"].asString());
  
  startAllAsync();
  
  while (!exitCommandUsed) {
    std::string userInput;
    std::getline(std::cin, userInput);
    std::vector<std::string> splitInput;
    boost::split(splitInput, userInput, boost::is_any_of(" "));
    
    if (splitInput.size() < 1) {
      std::cerr << "Error parsing user input" << std::endl;
    }	else if (splitInput[0] == "") {
      // The usger entered nothing, so just go back to waiting for input
      continue;
    } else if (splitInput[0] == "exit" || splitInput[0] == "q") {
      exitCommandUsed = true;
    } else if (splitInput[0] == "backup") {
      if (splitInput.size() < 2)
	std::cerr << "Usage: backup <fileName>" << std::endl;
    } else if (splitInput[0] == "rm") {
      if (splitInput.size() < 2)
	std::cerr << "Usage: rm <fileName>" << std::endl;
    } else if (splitInput[0] == "help") {
      std::cout << "Available commands: " << std::endl
		<< "exit              - exits the program." << std::endl
		<< "q                 - alias for exit." << std::endl
		<< "backup <fileName> - adds a file to backup." << std::endl
		<< "rm <fileName>     - removes a file that is already being "
		<< "backed up" << std::endl
		<< "help              - shows this help menu." << std::endl;
    } else {
      std::cerr << "Unknown command '" << splitInput[0] << "'. Type 'help' "
		<< "for a list of available commands." << std::endl;
    }
  }
  
  stopAllAsync();
}

void ConsoleController::createControllers() {
  asyncControllers_.push_back(
		std::shared_ptr<Controller>(
			new NetworkController(
				dispatcher_,
				NetworkHandlerFunction(handlePeerSocketConnection))));
  asyncControllers_.push_back(
		std::shared_ptr<Controller>(
			new BTSyncController(dispatcher_)));
}

void ConsoleController::startAllAsync() {
  for (std::vector<std::shared_ptr<Controller>>::iterator it =
				 asyncControllers_.begin();
       it != asyncControllers_.end();
       ++it)
    asyncControllerThreads_.push_back((*it)->startInBackground());
}

void ConsoleController::stopAllAsync() {
  for (std::vector<std::shared_ptr<Controller>>::iterator it =
				 asyncControllers_.begin();
			 it != asyncControllers_.end();
			 ++it)
    (*it)->stop();
  
  // Use a second loop so that the shutdown signal can be given to all threads
  // before trying to join any of them.
  for (std::vector<std::shared_ptr<std::thread>>::iterator it =
	 asyncControllerThreads_.begin();
	 it != asyncControllerThreads_.end();
	 ++it) {
    (*it)->join();
  }
}

bool ConsoleController::getTrackerInfo(Json::Value& configInfo) {
	std::ifstream configFile("config.json");
	bool result = false;
	
	if (configFile) {
		Json::Reader jsonReader;
		jsonReader.parse(configFile, configInfo);
		if (configInfo.isMember("TrackerIP") &&
				configInfo.isMember("TrackerPort") &&
				configInfo.isMember("BTSyncUsername") &&
				configInfo.isMember("BTSyncPassword") &&
				configInfo.isMember("BTSyncIP") &&
				configInfo.isMember("BTSyncPort") &&
				configInfo.isMember("BackupDirectory")) {
			result = true;
		}
	}
	
	return result;
}

} // namespace core
