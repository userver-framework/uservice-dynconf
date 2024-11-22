#pragma once

#include <memory>
#include <unordered_map>

#include <userver/cache/base_postgres_cache.hpp>
#include <userver/storages/postgres/io/chrono.hpp>

#include <models/config.hpp>

namespace uservice_dynconf::cache::settings_cache {

class ConfigCacheContainer {
public:
  using Key = uservice_dynconf::models::Key;
  using Config = uservice_dynconf::models::Config;
  using ConfigPtr = std::shared_ptr<const Config>;

  void insert_or_assign(Key &&key, Config &&config);
  size_t size() const;

  ConfigPtr FindConfig(const Key &key) const;
  std::vector<ConfigPtr> FindConfigsByService(std::string_view service) const;
  std::vector<ConfigPtr> FindConfigs(std::string_view service,
                                     const std::vector<std::string> &ids) const;

private:
  std::unordered_map<Key, ConfigPtr> configs_to_key_;
  std::unordered_map<std::string, std::vector<ConfigPtr>> configs_by_service_;
};

struct ConfigCachePolicy {
  static userver::storages::postgres::Query GetQuery();
  static constexpr auto kName = "configs-cache";
  using ValueType = uservice_dynconf::models::Config;
  using CacheContainer = ConfigCacheContainer;
  static constexpr auto kKeyMember = &uservice_dynconf::models::Config::key;
  static constexpr auto kUpdatedField = "updated_at";
  using UpdatedFieldType = userver::storages::postgres::TimePointTz;
};

using ConfigsCache = ::userver::components::PostgreCache<ConfigCachePolicy>;
} // namespace uservice_dynconf::cache::settings_cache
