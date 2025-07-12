#include <handlers/admin_v1_configs_get.hpp>

#include <string>

#include <userver/formats/json/value_builder.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

#include <docs/api/api.hpp>
#include <uservice_dynconf/sql_queries.hpp>

namespace uservice_dynconf::handlers::admin_v1_configs_get::post {

Handler::Handler(const userver::components::ComponentConfig &config,
                 const userver::components::ComponentContext &context)
    : HttpHandlerJsonBase(config, context),
      cluster_(
          context
              .FindComponent<userver::components::Postgres>("settings-database")
              .GetCluster()) {}

userver::formats::json::Value Handler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& /*request*/,
    const userver::formats::json::Value& /*request_json*/,
    userver::server::request::RequestContext&) const {
  namespace upg = userver::storages::postgres;

  userver::formats::json::ValueBuilder result;
  const auto data = cluster_->Execute(upg::ClusterHostType::kSlave,
                    uservice_dynconf::sql::kSelectConfigValues);
  for (const auto& row: data) {
      AdminConfigsValuesResponseBody element;
      element.name = row["config_name"].As<std::string>();
      element.service = row["service"].As<std::string>();
      element.value = row["config_value"].As<std::string>();
      element.is_kill_switch = row["is_kill_switch"].As<bool>();
      result.PushBack(std::move(element));
  }
  return result.ExtractValue();
}

} // namespace uservice_dynconf::handlers::admin_v1_configs_get::post
