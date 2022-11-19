#include "view.hpp"
#include "userver/server/http/http_status.hpp"
#include "userver/storages/postgres/cluster.hpp"
#include "userver/storages/postgres/component.hpp"

#include "sql/sql_query.hpp"
#include "utils/make_error.hpp"

namespace uservice_dynconf::handlers::configs_uuid::del {

Handler::Handler(const userver::components::ComponentConfig &config,
                 const userver::components::ComponentContext &context)
    : HttpHandlerJsonBase(config, context),
      cluster_(
          context
              .FindComponent<userver::components::Postgres>("settings-database")
              .GetCluster()) {}

userver::formats::json::Value Handler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest &request,
    const userver::formats::json::Value &,
    userver::server::request::RequestContext &) const {
  auto &http_response = request.GetHttpResponse();
  http_response.SetHeader("Access-Control-Allow-Origin", "*");
  
  if (request.GetMethod() != userver::server::http::HttpMethod::kDelete) {
    http_response.SetStatus(
        userver::server::http::HttpStatus::kMethodNotAllowed);
    return uservice_dynconf::utils::MakeError(
        "405", "Wrong method"); // limitation for setting, can't be tested
  }
  const auto request_uuid = request.GetPathArg("uuid");
  if (request_uuid == "") {
    http_response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    return uservice_dynconf::utils::MakeError("400", "No uuid providen");
  }

  auto query_result = cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      uservice_dynconf::sql::kDeleteConfig.data(), request_uuid);
  switch (query_result.RowsAffected()) {
  case 0: {
    http_response.SetStatus(userver::server::http::HttpStatus::kNotFound);
    return uservice_dynconf::utils::MakeError(
        "404", "Config with this uuid not exists");
  }
  case 1: {
    return {};
  }
  default: {
    http_response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    return uservice_dynconf::utils::MakeError(
        "500",
        "Providen uuid not exists. Have a nice day"); // Critical error
                                                      // checking
  }
  }
}

} // namespace uservice_dynconf::handlers::variables_uuid::del