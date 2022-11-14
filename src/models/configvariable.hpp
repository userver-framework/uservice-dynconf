#pragma once

#include <chrono>
#include <string>

#include <userver/formats/json/value_builder.hpp>

namespace uservice_dynconf::models {
struct ConfigVariable {
  std::string uuid;
  std::string service;
  std::string config_name;
  std::string config_value;
};

userver::formats::json::Value Serialize(const ConfigVariable &response,
          userver::formats::serialize::To<userver::formats::json::Value>);
}
