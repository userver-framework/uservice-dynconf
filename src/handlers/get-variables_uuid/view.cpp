#include "view.hpp"

#include "userver/formats/json/value.hpp"
#include "userver/server/http/http_status.hpp"
#include "userver/storages/postgres/cluster.hpp"
#include "userver/storages/postgres/component.hpp"

#include "sql/sql_query.hpp"
#include "utils/make_error.hpp"
#include "../../models/configvariable.hpp"

namespace uservice_dynconf::handlers::variables_uuid::get {

Handler::Handler(const userver::components::ComponentConfig &config,
                 const userver::components::ComponentContext &context)
    : HttpHandlerBase(config, context),
      cluster_(
          context
              .FindComponent<userver::components::Postgres>("settings-database")
              .GetCluster()) {}

std::string Handler::HandleRequestThrow(
    const userver::server::http::HttpRequest &request,
    userver::server::request::RequestContext &) const {
    auto& response = request.GetHttpResponse();
    userver::formats::json::ValueBuilder response_body;

    const auto& uuid = request.GetPathArg("uuid");
    
    auto result = cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
            "SELECT uuid, service, config_name, config_value::TEXT FROM uservice_dynconf.configs WHERE uuid=$1", uuid);
    if (result.IsEmpty()) {
        response.SetStatus(userver::server::http::HttpStatus::kNotFound);
        response_body["code"] = 404;
        response_body["message"] = "Not Found";
        return userver::formats::json::ToString(response_body.ExtractValue());
    }

    auto variable = result.AsSingleRow<uservice_dynconf::models::ConfigVariable>(userver::storages::postgres::kRowTag);
    response_body["uuid"] = variable.uuid;
    response_body["service"] = variable.service;
    response_body["config_name"] = variable.config_name;
    response_body["config_value"] = variable.config_value;
    return userver::formats::json::ToString(response_body.ExtractValue());
}

} // namespace uservice_dynconf::handlers::admin_v1_configs_delete::post
