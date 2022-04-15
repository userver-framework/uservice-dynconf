#pragma once

#include "userver/formats/json/value.hpp"
#include "userver/storages/postgres/io/chrono.hpp"
#include "userver/utils/strong_typedef.hpp"
#include <boost/container_hash/hash.hpp>
#include <boost/functional/hash.hpp>
#include <chrono>
#include <iterator>
#include <string>
namespace service_dynamic_configs::models {

using ConfigId = userver::utils::StrongTypedef<
    class ConfigIdTag, int64_t,
    userver::utils::StrongTypedefOps::kCompareTransparent>;

struct Key {
  std::string service;
  std::string config_name;

  bool operator==(const Key &other) const {
    return std::tie(service, config_name) ==
           std::tie(other.service, other.config_name);
  }
};

struct Config {
  Key key;
  userver::formats::json::Value config_value;
  userver::storages::postgres::TimePointTz updated_at;
};
} // namespace service_dynamic_configs::models

namespace std {

template <> struct hash<service_dynamic_configs::models::Key> {
  size_t operator()(const service_dynamic_configs::models::Key &param) const {
    size_t seed = 0;
    boost::hash_combine(seed, param.service);
    boost::hash_combine(seed, param.config_name);
    return seed;
  }
};
} // namespace std
