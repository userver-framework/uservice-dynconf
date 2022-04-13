#include "settings_cache.hpp"
#include "userver/logging/log.hpp"
#include "userver/storages/postgres/query.hpp"

#include "sql/sql_query.hpp"
#include "userver/utils/algo.hpp"

namespace service_synamic_configs::cache::settings_cache {

namespace {
constexpr static const char *kDefaultService = "__default__";
}

const userver::storages::postgres::Query &ConfigCachePolicy::kQuery =
    userver::storages::postgres::Query(
        service_dynamic_configs::sql::kSelectSettingsForCache);

void ConfigCacheContainer::insert_or_assign(Key &&key, Config &&config) {
  auto config_ptr = std::make_shared<const Config>(std::move(config));
  configs_to_key_.insert_or_assign(key, config_ptr);
  configs_by_service_[key.service].push_back(std::move(config_ptr));
}

std::vector<ConfigCacheContainer::ConfigPtr>
ConfigCacheContainer::FindConfigsByService(std::string_view service) const {
  if (configs_by_service_.count(service.data()) > 0) {
    return configs_by_service_.at(service.data());
  }
  return {};
}

size_t ConfigCacheContainer::size() const { return configs_to_key_.size(); }

ConfigCacheContainer::ConfigPtr
ConfigCacheContainer::FindConfig(const ConfigCacheContainer::Key &key) const {
  if (auto c_ptr = userver::utils::FindOrDefault(configs_to_key_, key, nullptr);
      c_ptr) {
    return c_ptr;
  }
  return userver::utils::FindOrDefault(
      configs_to_key_,
      ConfigCacheContainer::Key{{kDefaultService}, key.config_name}, nullptr);
}

} // namespace service_synamic_configs::cache::settings_cache
