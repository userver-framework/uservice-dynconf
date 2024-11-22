#pragma once

#include <string>

#include <boost/functional/hash.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/storages/postgres/io/chrono.hpp>
#include <userver/storages/postgres/io/enum_types.hpp>
#include <userver/utils/strong_typedef.hpp>
#include <userver/utils/trivial_map.hpp>

namespace uservice_dynconf::models {

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

enum class Mode { kDynamicConfig, kKillSwitchEnabled, kKillSwitchDisabled };

std::string ToString(Mode mode);

struct Config {
  Key key;
  userver::formats::json::Value config_value;
  Mode mode;
  userver::storages::postgres::TimePointTz updated_at;
};
} // namespace uservice_dynconf::models

namespace std {

template <> struct hash<uservice_dynconf::models::Key> {
  size_t operator()(const uservice_dynconf::models::Key &param) const {
    size_t seed = 0;
    boost::hash_combine(seed, param.service);
    boost::hash_combine(seed, param.config_name);
    return seed;
  }
};
} // namespace std

USERVER_NAMESPACE_BEGIN

template <>
struct storages::postgres::io::CppToUserPg<uservice_dynconf::models::Mode> {
  static constexpr DBTypeName postgres_name = "uservice_dynconf.mode";
  static constexpr userver::utils::TrivialBiMap enumerators =
      [](auto selector) {
        using Mode = uservice_dynconf::models::Mode;
        return selector()
            .Case(uservice_dynconf::models::ToString(Mode::kDynamicConfig),
                  Mode::kDynamicConfig)
            .Case(uservice_dynconf::models::ToString(Mode::kKillSwitchEnabled),
                  Mode::kKillSwitchEnabled)
            .Case(uservice_dynconf::models::ToString(Mode::kKillSwitchDisabled),
                  Mode::kKillSwitchDisabled);
      };
};

USERVER_NAMESPACE_END
