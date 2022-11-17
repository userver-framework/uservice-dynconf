#include "view.hpp"

#include "userver/formats/json/value.hpp"
#include "userver/server/http/http_status.hpp"
#include "userver/storages/postgres/cluster.hpp"
#include "userver/storages/postgres/component.hpp"

#include "models/configvariable.hpp"
#include "sql/sql_query.hpp"
#include "utils/make_error.hpp"

namespace uservice_dynconf::handlers::variables_uuid::get {

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
  http_response.SetHeader("Access-Control-Allow-Origin", "*");
  
  userver::formats::json::ValueBuilder response_body;

  const auto &uuid = request.GetPathArg("uuid");

  auto result = cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      uservice_dynconf::sql::kSelectVariableWithValue.data(), uuid);

  if (result.IsEmpty()) {
    http_response.SetStatus(userver::server::http::HttpStatus::kNotFound);
    return userver::formats::json::ToString(
        uservice_dynconf::utils::MakeError("404", "Not Found"));
  }

  auto variable = result.AsSingleRow<uservice_dynconf::models::ConfigVariable>(
      userver::storages::postgres::kRowTag);
  response_body["uuid"] = variable.uuid;
  response_body["service"] = variable.service;
  response_body["config_name"] = variable.config_name;
  response_body["config_value"] = variable.config_value;
  return userver::formats::json::ToString(response_body.ExtractValue());
}

} // namespace uservice_dynconf::handlers::variables_uuid::get
