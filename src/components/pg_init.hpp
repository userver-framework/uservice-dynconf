#pragma once

#include <userver/components/loggable_component_base.hpp>

#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

namespace service_dynamic_configs::components::pg_init {

class Component final : public userver::components::LoggableComponentBase {
public:
  static constexpr auto kName = "pg-init";

  Component(const userver::components::ComponentConfig &config,
            const userver::components::ComponentContext &context);

private:
  userver::storages::postgres::ClusterPtr cluster_;
};

} // namespace service_dynamic_configs::components::pg_init
