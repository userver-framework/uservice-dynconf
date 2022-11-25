#include "view.hpp"

#include "userver/formats/json/value.hpp"
#include "userver/server/http/http_status.hpp"
#include "userver/storages/postgres/cluster.hpp"
#include "userver/storages/postgres/component.hpp"

#include "sql/sql_query.hpp"
#include "utils/make_error.hpp"

namespace uservice_dynconf::handlers::configs_uuid::get {

struct DBData {
  std::string uuid;
  std::string service;
  std::string config_name;
  std::optional<std::string> config_value;
};

Handler::Handler(const userver::components::ComponentConfig &config,
                 const userver::components::ComponentContext &context)
    : HttpHandlerBase(config, context),
      cluster_(
          context
              .FindComponent<userver::components::Postgres>("settings-database")
              .GetCluster()) {}

std::string
Handler::HandleRequestThrow(const userver::server::http::HttpRequest &request,
                            userver::server::request::RequestContext &) const {
  auto &http_response = request.GetHttpResponse();
  http_response.SetHeader("Content-Type", "application/json");
  http_response.SetHeader("Access-Control-Allow-Origin", "*");

  userver::formats::json::ValueBuilder response_body;

  const auto &uuid = request.GetPathArg("uuid");

  auto result = cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      uservice_dynconf::sql::kSelectConfigWithValue.data(), uuid);

  if (result.IsEmpty()) {
    http_response.SetStatus(userver::server::http::HttpStatus::kNotFound);
    return userver::formats::json::ToString(
        uservice_dynconf::utils::MakeError("404", "Not Found"));
  }

  auto config =
      result.AsSingleRow<DBData>(userver::storages::postgres::kRowTag);
  response_body["uuid"] = config.uuid;
  response_body["servic_name"] = config.service;
  response_body["config_name"] = config.config_name;
  if (config.config_value.has_value())
    response_body["config_value"] = config.config_value.value();
  else
    response_body["config_value"] = "null";
  return userver::formats::json::ToString(response_body.ExtractValue());
}

} // namespace uservice_dynconf::handlers::configs_uuid::get
