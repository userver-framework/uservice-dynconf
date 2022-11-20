#pragma once

#include "userver/formats/json/value.hpp"
#include "userver/storages/postgres/io/chrono.hpp"
#include "userver/utils/strong_typedef.hpp"
#include <boost/functional/hash.hpp>
#include <chrono>
#include <iterator>
#include <string>

namespace uservice_dynconf::models {
using ConfigId = userver::utils::StrongTypedef<
    class ConfigIdTag, int64_t,
    userver::utils::StrongTypedefOps::kCompareTransparent>;

struct Config {
  std::string key;
  std::string service_uuid;
  std::string config_name;
  userver::formats::json::Value config_value;
  userver::storages::postgres::TimePointTz updated_at;
};
} // namespace uservice_dynconf::models