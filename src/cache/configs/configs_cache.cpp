#include "configs_cache.hpp"
#include "userver/logging/log.hpp"
#include "userver/storages/postgres/query.hpp"

#include "sql/sql_query.hpp"
#include "userver/utils/algo.hpp"
#include <string_view>

namespace uservice_dynconf::cache::settings_cache {

namespace {
constexpr static const char *kDefaultService = "__default__";
constexpr static const char *kDefaultServiceUuid = "__default_uuid__";
}

userver::storages::postgres::Query ConfigCachePolicy::kQuery =
    userver::storages::postgres::Query(
        uservice_dynconf::sql::kSelectConfigForCache.data());

void ConfigCacheContainer::insert_or_assign(Key &&key, Config &&config) {
  auto config_ptr = std::make_shared<const Config>(std::move(config));
  configs_to_key_.insert_or_assign(key, config_ptr);
  configs_to_name_.insert_or_assign(config_ptr->config_name, config_ptr);
  configs_by_service_[config_ptr->service_uuid].push_back(std::move(config_ptr));
}

size_t ConfigCacheContainer::size() const { return configs_to_key_.size(); }

std::vector<ConfigCacheContainer::ConfigPtr>
ConfigCacheContainer::FindConfigsByService(std::string service_uuid) const {
  if (auto it = configs_by_service_.find(service_uuid);
      it != configs_by_service_.end()) {
    return it->second;
  }
  return {};
}

std::vector<ConfigCacheContainer::ConfigPtr>
ConfigCacheContainer::FindConfigs(const std::string service_uuid,
                                  const std::vector<std::string> &ids) const {
  std::vector<ConfigCacheContainer::ConfigPtr> result{};
  result.reserve(ids.size());

  for (const auto &id : ids) {
    auto c_ptr = userver::utils::FindOrDefault(configs_to_name_, id, nullptr);
    if (c_ptr) {
      if (c_ptr->service_uuid == service_uuid) {
        result.emplace_back(c_ptr);
        continue;
      } else if (c_ptr->service_uuid == kDefaultServiceUuid) {
        result.emplace_back(c_ptr);
      }
    }
  }
  return result;
}

} // namespace uservice_dynconf::cache::settings_cache
