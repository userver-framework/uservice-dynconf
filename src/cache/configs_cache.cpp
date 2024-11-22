#include <cache/configs_cache.hpp>

#include <string_view>

#include <userver/logging/log.hpp>
#include <userver/storages/postgres/query.hpp>
#include <userver/utils/algo.hpp>
#include <uservice_dynconf/sql_queries.hpp>

namespace uservice_dynconf::cache::settings_cache {

namespace {
constexpr static const char *kDefaultService = "__default__";
}

userver::storages::postgres::Query ConfigCachePolicy::GetQuery() {
  return uservice_dynconf::sql::kSelectSettingsForCache;
}

void ConfigCacheContainer::insert_or_assign(Key &&key, Config &&config) {
  auto config_ptr = std::make_shared<const Config>(std::move(config));
  configs_to_key_.insert_or_assign(key, config_ptr);
  configs_by_service_[key.service].push_back(std::move(config_ptr));
}

std::vector<ConfigCacheContainer::ConfigPtr>
ConfigCacheContainer::FindConfigsByService(std::string_view service) const {
  if (auto it = configs_by_service_.find(service.data());
      it != configs_by_service_.end()) {
    return it->second;
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

std::vector<ConfigCacheContainer::ConfigPtr>
ConfigCacheContainer::FindConfigs(std::string_view service,
                                  const std::vector<std::string> &ids) const {
  std::vector<ConfigCacheContainer::ConfigPtr> result{};
  result.reserve(ids.size());
  for (const auto &id : ids) {
    if (auto c_ptr = userver::utils::FindOrDefault(
            configs_to_key_, Key{service.data(), id}, nullptr);
        c_ptr) {
      result.emplace_back(c_ptr);
      continue;
    }
    if (auto c_ptr = userver::utils::FindOrDefault(
            configs_to_key_, ConfigCacheContainer::Key{{kDefaultService}, id},
            nullptr);
        c_ptr) {
      result.emplace_back(c_ptr);
    }
  }
  return result;
}

} // namespace uservice_dynconf::cache::settings_cache
