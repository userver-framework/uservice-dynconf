#include "view.hpp"
#include <iostream>

#include "userver/formats/json/value.hpp"
#include "userver/server/http/http_status.hpp"
#include "userver/storages/postgres/cluster.hpp"
#include "userver/storages/postgres/component.hpp"

#include "sql/sql_query.hpp"
#include "utils/make_error.hpp"

namespace uservice_dynconf::handlers::configs_uuid::patch {

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
  auto request_body = userver::formats::json::FromString(request.RequestBody());
  auto value = request_body["value"].As<std::optional<std::string>>();
  if (!value.has_value()) {
    http_response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    return userver::formats::json::ToString(uservice_dynconf::utils::MakeError(
        "400", "no 'value' field in request"));
  }

  try {
    auto val = userver::formats::json::FromString(value.value());
  } catch (const userver::formats::json::ParseException &e) {
    http_response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    return userver::formats::json::ToString(uservice_dynconf::utils::MakeError(
        "400", "'value' field is not a json object"));
  }

  auto result = cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      uservice_dynconf::sql::kUpdateConfig.data(), uuid, value.value());
  if (result.IsEmpty()) {
    http_response.SetStatus(userver::server::http::HttpStatus::kNotFound);
    return userver::formats::json::ToString(
        uservice_dynconf::utils::MakeError("404", "Not Found"));
  }

  return {};
}

} // namespace uservice_dynconf::handlers::configs_uuid::patch
