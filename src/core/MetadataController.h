
#ifndef CORE_METADATA_CONTROLLER_H_
#define CORE_METADATA_CONTROLLER_H_

#include "Controller.h"

namespace core {

class MetadataController : public Controller {
 public:
  MetadataController(std::shared_ptr<Dispatcher> dispatcher);
  ~MetadataController();
  void start();
  void stop();
 private:
  bool shouldStop_;
}; // class MetadataController

} // namespace core

#endif // CORE_METADATA_CONTROLLER_H_
