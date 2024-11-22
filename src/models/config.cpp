#include <models/config.hpp>

#include <cassert>
#include <string>

namespace uservice_dynconf::models {

std::string ToString(Mode mode) {
  switch (mode) {
  case Mode::kDynamicConfig: {
    return "dynamic_config";
  }
  case Mode::kKillSwitchEnabled: {
    return "kill_switch_enabled";
  }
  case Mode::kKillSwitchDisabled: {
    return "kill_switch_disabled";
  }
  default: {
    assert(false); // not expected case
    return "unknown";
  }
  }
};

}; // namespace uservice_dynconf::models
