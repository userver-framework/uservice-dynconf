#include "admin_v1_configs_delete.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/server/http/http_status.hpp"
#include "userver/storages/postgres/cluster.hpp"
#include "userver/storages/postgres/component.hpp"

#include "sql/sql_query.hpp"

namespace service_dynamic_configs::handlers::admin_v1_configs_delete::post {

namespace {

struct RequestData {
  std::vector<std::string> ids{};
  std::string service{};
};

RequestData ParseRequest(const userver::formats::json::Value &request) {
  RequestData result;
  result.ids = request["ids"].As<std::vector<std::string>>({});
  result.service = request["service"].As<std::string>({});
  return result;
}

userver::formats::json::Value MakeError(std::string_view code,
                                        std::string_view message) {
  userver::formats::json::ValueBuilder builder;
  builder["code"] = code;
  builder["message"] = message;
  return builder.ExtractValue();
}

} // namespace

Handler::Handler(const userver::components::ComponentConfig &config,
                 const userver::components::ComponentContext &context)
    : HttpHandlerJsonBase(config, context),
      cluster_(
          context
              .FindComponent<userver::components::Postgres>("settings-database")
              .GetCluster()) {}

userver::formats::json::Value Handler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest &request,
    const userver::formats::json::Value &request_json,
    userver::server::request::RequestContext & /*context*/) const {
  const auto request_data = ParseRequest(request_json);
  auto &http_response = request.GetHttpResponse();
  if (request_data.ids.empty() || request_data.service.empty()) {
    http_response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    return MakeError("400", "Fields ids and service requred");
  }

  cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                    service_dynamic_configs::sql::kDeleteConfigValues,
                    request_data.service, request_data.ids);

  http_response.SetStatus(userver::server::http::HttpStatus::kNoContent);
  return {};
}

} // namespace service_dynamic_configs::handlers::admin_v1_configs_delete::post
