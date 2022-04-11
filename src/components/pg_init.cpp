#include "pg_init.hpp"
#include "userver/components/component_context.hpp"
#include "userver/storages/postgres/cluster.hpp"
#include "userver/storages/postgres/cluster_types.hpp"
#include "userver/storages/postgres/component.hpp"
#include <userver/logging/log.hpp>

#include "sql/sql_query.hpp"

namespace service_dynamic_configs::components::pg_init {

Component::Component(const userver::components::ComponentConfig &config,
                     const userver::components::ComponentContext &context)
    : LoggableComponentBase(config, context),
      cluster_(
          context
              .FindComponent<userver::components::Postgres>("settings-database")
              .GetCluster()) {
  auto trx = cluster_->Begin(
      "init_database", userver::storages::postgres::ClusterHostType::kMaster,
      {});

  trx.Execute(sql::kCreateSchema);
  trx.Execute(sql::kCreateTable);
  trx.Execute(sql::kCreateIndexCreated);
  trx.Execute(sql::kCreateIndexUpdate);
  trx.Execute(sql::kCreateIndexPair);
  trx.Execute(sql::kInsertDefaultConfigs);
  trx.Commit();
}

} // namespace service_dynamic_configs::components::pg_init
