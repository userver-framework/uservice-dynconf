#pragma once

#include <memory>
#include <unordered_map>
#include <userver/cache/base_postgres_cache.hpp>
#include <userver/storages/postgres/io/chrono.hpp>

#include "models/service.hpp"

namespace uservice_dynconf::cache::settings_cache {

class ServiceCacheContainer {
public:
  using Key = uservice_dynconf::models::KeyService;
  using Service = uservice_dynconf::models::Service;
  using ServicePtr = std::shared_ptr<const Service>;

  void insert_or_assign(Key &&key, Service &&service);
  size_t size() const;
  
  ServicePtr FindServiceByName(std::string_view service_name) const;
  ServicePtr FindService(const Key key) const;

private:
  std::unordered_map<Key, ServicePtr> service_by_uuid_;
  std::unordered_map<std::string, ServicePtr> service_by_name_;
};

struct ServiceCachePolicy {
  static constexpr auto kName = "services-cache";
  using ValueType = uservice_dynconf::models::Service;
  using CacheContainer = ServiceCacheContainer;
  static constexpr auto kKeyMember = &uservice_dynconf::models::Service::key;
  static userver::storages::postgres::Query kQuery;
  static constexpr auto kUpdatedField = "updated_at";
  using UpdatedFieldType = userver::storages::postgres::TimePointTz;
};

using ServicesCache = ::userver::components::PostgreCache<ServiceCachePolicy>;
} // namespace uservice_dynconf::cache::settings_cache
