#include "view.hpp"

#include "sql/sql_query.hpp"
#include "userver/formats/json/inline.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/formats/yaml/value_builder.hpp"
#include "utils/make_error.hpp"
#include <userver/formats/json.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

namespace uservice_dynconf::handlers::services_uuid_clone::post {

struct RequestData {
  std::string config_name;
  std::optional<std::string> config_value;
};

Handler::Handler(const userver::components::ComponentConfig &config,
                 const userver::components::ComponentContext &component_context)
    : HttpHandlerJsonBase(config, component_context),
      cluster_(
          component_context
              .FindComponent<userver::components::Postgres>("settings-database")
              .GetCluster()) {}

userver::formats::json::Value Handler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest &request,
    const userver::formats::json::Value &json,
    userver::server::request::RequestContext &) const {
  const auto &uuid = request.GetPathArg("uuid");
  const auto &new_service_name =
      json["service_name"].As<std::optional<std::string>>();

  auto &http_response = request.GetHttpResponse();
  http_response.SetHeader("Content-Type", "application/json");
  http_response.SetHeader("Access-Control-Allow-Origin", "*");

  if (uuid.empty() || new_service_name.value_or("") == "") {
    http_response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    return uservice_dynconf::utils::MakeError(
        "400", "Field 'uuid' or 'service' is empty");
  }

  auto result = cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kSlave,
      uservice_dynconf::sql::kSelectServiceToClone.data(), uuid);

  if (result.IsEmpty()) {
    http_response.SetStatusNotFound();
    return uservice_dynconf::utils::MakeError("404", "Service not found");
  }

  const auto copied_service =
      result.AsSingleRow<RequestData>(userver::storages::postgres::kRowTag);

  result =
      cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                        uservice_dynconf::sql::kInsertClonedService.data(),
                        new_service_name.value());

  if (result.IsEmpty()) {
    http_response.SetStatus(userver::server::http::HttpStatus::kConflict);
    return uservice_dynconf::utils::MakeError(
        "409", "Service with that name already exists");
  }

  auto service_uuid = result.AsSingleRow<std::string>();
  result = cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      uservice_dynconf::sql::kInsertClonedConfigs.data(), service_uuid,
      copied_service.config_name, copied_service.config_value);

  if (result.IsEmpty()) {
    http_response.SetStatus(userver::server::http::HttpStatus::kConflict);
    return uservice_dynconf::utils::MakeError(
        "500", "Couldn't insert config. Very bad");
  }

  http_response.SetStatusOk();
  userver::formats::json::ValueBuilder response;
  response["config_uuid"] = result.AsSingleRow<std::string>();
  return response.ExtractValue();
}
} // namespace uservice_dynconf::handlers::services_uuid_clone::post
