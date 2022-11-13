#include "view.hpp"
#include <iostream>

#include "userver/formats/json/value.hpp"
#include "userver/server/http/http_status.hpp"
#include "userver/storages/postgres/cluster.hpp"
#include "userver/storages/postgres/component.hpp"

#include "sql/sql_query.hpp"
#include "utils/make_error.hpp"
#include "../../models/configvariable.hpp"

namespace uservice_dynconf::handlers::variables_uuid::patch {

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
    auto request_body = userver::formats::json::FromString(request.RequestBody());
    auto value = request_body["value"].As<std::optional<std::string>>();
    if (!value.has_value()) {
        response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
        response_body["code"] = 400;
        response_body["message"] = "Bad Request";
        return userver::formats::json::ToString(response_body.ExtractValue());
    }

    try {
        auto result = cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kMaster,
                "UPDATE uservice_dynconf.configs SET config_value=$2::JSONB, updated_at = now() WHERE uuid=$1 RETURNING uuid", uuid, value.value());
        if (result.IsEmpty()) {
            response.SetStatus(userver::server::http::HttpStatus::kNotFound);
            response_body["code"] = 404;
            response_body["message"] = "Not Found";
            return userver::formats::json::ToString(response_body.ExtractValue());
        }
    } catch (std::exception& e) {
        std::cout << "\n\n" <<  e.what()  << "\n\n" << std::endl;
    }

    response_body["code"] = 200;
    return {};
}

} // namespace uservice_dynconf::handlers::admin_v1_configs_delete::post
