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

namespace uservice_dynconf::handlers::configs::post {
    namespace {
        struct RequestData {
            std::optional<std::string> config_value{};
            std::string config_name{};
            std::string service_name{};
            bool isValid() const { return !config_name.empty(); }
        };

        RequestData ParseRequest(const userver::formats::json::Value &request) {
            RequestData result;
            result.config_value = request["value"].As<std::optional<std::string>>();
            if (result.config_value.has_value() &&
                result.config_value.value() == "null") {
                result.config_value.reset();
            }
            result.config_name = request["name"].As<std::string>({});
            result.service_name = request["service"].As<std::string>("__default__");
            return result;
        }
    } // namespace
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
        auto &http_response = request.GetHttpResponse();
        const auto request_data = ParseRequest(json);
        http_response.SetHeader("Access-Control-Allow-Origin", "*");
        http_response.SetHeader("Content-Type", "application/json");
        if (!request_data.isValid()) {
            http_response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
            return uservice_dynconf::utils::MakeError("400", "Config name is required");
        }
        try {
            userver::formats::json::FromString(
                    request_data.config_value.value_or("null"));
        } catch (const userver::formats::json::ParseException &e) {
            http_response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
            return uservice_dynconf::utils::MakeError("400", e.what());
        }
        auto result = cluster_->Execute(
                userver::storages::postgres::ClusterHostType::kMaster,
                uservice_dynconf::sql::kInsertConfig.data(),
                request_data.service_name, request_data.config_name,
                request_data.config_value);
        if (result.IsEmpty()) {
            http_response.SetStatus(userver::server::http::HttpStatus::kConflict);
            return uservice_dynconf::utils::MakeError(
                    "409", "Config variable already exists for that service");
        }
        userver::formats::json::ValueBuilder response;
        response["uuid"] = result.AsSingleRow<std::string>();
        return response.ExtractValue();
    }
} // namespace uservice_dynconf::handlers::variables::post