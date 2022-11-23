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

namespace uservice_dynconf::handlers::configs_uuid_clone::post {

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
      
  const auto &config_uuid = request.GetPathArg("uuid");
  const auto &service_name =
      json["service_name"].As<std::optional<std::string>>();
  const auto& inConfig_name = 
      json["config_name"].As<std::optional<std::string>>();
  const auto& inConfig_value = 
      json["config_value"].As<std::optional<std::string>>();

  auto &http_response = request.GetHttpResponse();
  http_response.SetHeader("Content-Type", "application/json");
  http_response.SetHeader("Access-Control-Allow-Origin", "*");

  if (config_uuid.empty() || service_name.value_or("") == "") {
    http_response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    return uservice_dynconf::utils::MakeError(
        "400", "Field 'uuid' or 'service_name' is empty");
  }

  auto config = cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kSlave,
      uservice_dynconf::sql::kSelectConfigToClone.data(), config_uuid);
  if (config.IsEmpty()) {
    http_response.SetStatusNotFound();
    return uservice_dynconf::utils::MakeError("404", "Config not found");
  }    

  auto service = cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kSlave,
      uservice_dynconf::sql::kSelectService.data(), service_name.value());
  if (service.IsEmpty()) {
    service =
        cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                          uservice_dynconf::sql::kInsertService.data(),
                          service_name.value());
  }
  if (service.IsEmpty()) {
    http_response.SetStatus(userver::server::http::HttpStatus::kConflict);
    return uservice_dynconf::utils::MakeError(
        "500", "Could not insert service. INSERT CONFICT. Very bad");
  }
  const auto& service_uuid = service.AsSingleRow<std::string>();
  auto copied_config =
      config.AsSingleRow<RequestData>(userver::storages::postgres::kRowTag);
  {
    if(inConfig_name.value_or("") != "")
      copied_config.config_name = inConfig_name.value();
    if(inConfig_value.value_or("") != "")
      copied_config.config_value = inConfig_value.value();
  }
  
  auto result = cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      uservice_dynconf::sql::kInsertClonedConfig.data(), service_uuid,
      copied_config.config_name, copied_config.config_value);
  if (result.IsEmpty()) {
    http_response.SetStatus(userver::server::http::HttpStatus::kConflict);
    return uservice_dynconf::utils::MakeError(
        "409", "Service with that config_name already exists");
  }

  http_response.SetStatusOk();
  userver::formats::json::ValueBuilder response;
  response["config_uuid"] = result.AsSingleRow<std::string>();
  response["service_uuid"] = service_uuid;
  return response.ExtractValue();
}
} // namespace uservice_dynconf::handlers::configs_uuid_clone::post
