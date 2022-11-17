#include "services_cache.hpp"
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

userver::storages::postgres::Query ServiceCachePolicy::kQuery =
    userver::storages::postgres::Query(
        uservice_dynconf::sql::kSelectServiceForCache.data());

void ServiceCacheContainer::insert_or_assign(Key &&key, Service &&service) {
    auto service_ptr = std::make_shared<const Service>(std::move(service));
    service_by_uuid_.insert_or_assign(key, service_ptr);
    service_by_name_.insert_or_assign(service.service_name, service_ptr);
}

size_t ServiceCacheContainer::size() const { return service_by_uuid_.size(); }

ServicePtr FindServiceByName(std::string_view service_name) const  {
    if (auto c_ptr = userver::utils::FindOrDefault(service_by_name_, service_name, nullptr); c_ptr) {
        return c_ptr;
    }
    return userver::utils::FindOrDefault(
        service_by_name_, kDefaultService, nullptr);
}

ServicePtr FindService(std::string_view service_uuid) const {
    if (auto c_ptr = userver::utils::FindOrDefault(service_by_uuid_, service_uuid, nullptr);
        c_ptr) {
    return c_ptr;
    }
    return userver::utils::FindOrDefault(service_by_uuid_, kDefaultServiceUuid, nullptr);
}

} // namespace uservice_dynconf::cache::settings_cache
