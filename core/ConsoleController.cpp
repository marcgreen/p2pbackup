
#include "core/BTSyncController.h"
#include "core/ConsoleController.h"
#include "core/Controller.h"
#include "core/Dispatcher.h"
#include "core/MetadataController.h"
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

void ConsoleController::start(const std::string& localBackupInfoLocation) {
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
     configInfo["BackupDirectory"].asString(),
     localBackupInfoLocation);
  
  if (!peer.joinNetwork()) {
    std::cerr << "Fatal error: could not join network" << std::endl;
    return;
  }
  
  startAllAsync();
  
  while (!exitCommandUsed) {
    std::cout << "> " << std::endl;
    std::string userInput;
    std::getline(std::cin, userInput);
    std::vector<std::string> splitInput;
    boost::split(splitInput, userInput, boost::is_any_of(" "));
    
    if (splitInput.size() < 1) {
      std::cerr << "Error parsing user input" << std::endl;
    }	else if (splitInput[0] == "") {
      // The user entered nothing, so just go back to waiting for input
      continue;
    } else if (splitInput[0] == "exit" || splitInput[0] == "q") {
      exitCommandUsed = true;
    } else if (splitInput[0] == "backup") {
      if (splitInput.size() < 2)
	std::cerr << "Usage: backup <fileName>" << std::endl;
      peer.backupFile(splitInput[1]);
    } else if (splitInput[0] == "rm") {
      if (splitInput.size() < 2)
	std::cerr << "Usage: rm <fileName>" << std::endl;
      peer.removeBackup(splitInput[1]);
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

void ConsoleController::start() {
  start(std::string());
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
  asyncControllers_.push_back(
    std::shared_ptr<Controller>(
      new MetadataController(dispatcher_)));
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
    result = true;
    if (!configInfo.isMember("TrackerIP")) {
      std::cout << "Didn't find TrackerIP" << std::endl;
      result = false;
    }
    if (!configInfo.isMember("TrackerPort")) {
      std::cout << "Didn't find TrackerPort" << std::endl;
      result = false;
    }
    if (!configInfo.isMember("BTSyncUsername")) {
      std::cout << "Didn't find BTSyncUsername" << std::endl;
      result = false;
    }
    if (!configInfo.isMember("BTSyncPassword")) {
      std::cout << "Didn't find BTSyncPassword" << std::endl;
      result = false;
    }
    if (!configInfo.isMember("BTSyncIP")) {
      std::cout << "Didn't find BTSyncIP" << std::endl;
      result = false;
    }
    if (!configInfo.isMember("BTSyncPort")) {
      std::cout << "Didn't find BTSyncPort" << std::endl;
      result = false;
    }
    if (!configInfo.isMember("BackupDirectory")) {
      std::cout << "Didn't find BackupDirectory" << std::endl;
      result = false;
    }
    
    if (!result) {
      std::cout << "Config was found but is invalid" << std::endl;
    }
  } else {
    std::cout << "File could not be opened" << std::endl;
  }
  
  return result;
}

} // namespace core
