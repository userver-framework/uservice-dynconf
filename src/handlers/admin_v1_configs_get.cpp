#include <handlers/admin_v1_configs_get.hpp>

#include <string>
#include <unordered_set>

#include <userver/formats/json/inline.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/formats/yaml/value_builder.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

#include <docs/api/api.hpp>
#include <uservice_dynconf/sql_queries.hpp>

#include <models/config.hpp>
#include <utils/make_error.hpp>

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

  userver::formats::json::ValueBuilder result;
  const auto data = cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                    uservice_dynconf::sql::kSelectConfigValues);
  for (const auto& row: data) {
      result.PushBack( 
        userver::formats::json::MakeObject(
            "name", row["config_name"].As<std::string>(),
            "service", row["service"].As<std::string>(),
            "value", row["config_value"].As<std::string>()
        )
      );
  }
  return result.ExtractValue();
}

} // namespace uservice_dynconf::handlers::admin_v1_configs_get::post
