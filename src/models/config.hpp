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

struct KeyConfig {
  std::string uuid;

  bool operator==(const KeyConfig &other) const {
    return std::tie(uuid) ==
           std::tie(other.uuid);
  }
};

struct Config {
  KeyConfig key;
  std::string service_uuid;
  std::string config_name;
  userver::formats::json::Value config_value;
  userver::storages::postgres::TimePointTz updated_at;
};
} // namespace uservice_dynconf::models

namespace std {

template <> struct hash<uservice_dynconf::models::KeyConfig> {
  size_t operator()(const uservice_dynconf::models::KeyConfig &param) const {
    size_t seed = 0;
    boost::hash_combine(seed, param.uuid);
    return seed;
  }
};
} // namespace std