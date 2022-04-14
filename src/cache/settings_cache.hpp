#pragma once

#include <memory>
#include <unordered_map>
#include <userver/cache/base_postgres_cache.hpp>
#include <userver/storages/postgres/io/chrono.hpp>

#include "models/config.hpp"

namespace service_dynamic_configs::cache::settings_cache {

class ConfigCacheContainer {
public:
  using Key = service_dynamic_configs::models::Key;
  using Config = service_dynamic_configs::models::Config;
  using ConfigPtr = std::shared_ptr<const Config>;

  void insert_or_assign(Key &&key, Config &&config);
  size_t size() const;

  ConfigPtr FindConfig(const Key &key) const;
  std::vector<ConfigPtr> FindConfigsByService(std::string_view service) const;

private:
  std::unordered_map<Key, ConfigPtr> configs_to_key_;
  std::unordered_map<std::string, std::vector<ConfigPtr>> configs_by_service_;
};

struct ConfigCachePolicy {
  static constexpr auto kName = "configs-cache";
  using ValueType = service_dynamic_configs::models::Config;
  using CacheContainer = ConfigCacheContainer;
  static constexpr auto kKeyMember =
      &service_dynamic_configs::models::Config::key;
  static userver::storages::postgres::Query kQuery;
  static constexpr auto kUpdatedField = "updated_at";
  using UpdatedFieldType = userver::storages::postgres::TimePointTz;
};

using ConfigsCache = ::userver::components::PostgreCache<ConfigCachePolicy>;
} // namespace service_dynamic_configs::cache::settings_cache
