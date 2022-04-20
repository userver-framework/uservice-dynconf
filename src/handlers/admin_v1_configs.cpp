#include "admin_v1_configs.hpp"
#include "userver/formats/json/inline.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/formats/yaml/value_builder.hpp"
#include "userver/storages/postgres/cluster.hpp"
#include "userver/storages/postgres/component.hpp"

#include "sql/sql_query.hpp"
#include "utils/make_error.hpp"
#include <vector>

namespace uservice_dynconf::handlers::admin_v1_configs::post {

namespace {
struct RequestData {
  userver::formats::json::Value configs;
  std::string service{};
};

RequestData ParseRequest(const userver::formats::json::Value &request) {
  RequestData result;
  if (request["configs"].IsObject()) {
    result.configs = request["configs"];
  }
  result.service = request["service"].As<std::string>({});
  return result;
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
    userver::server::request::RequestContext &) const {
  auto &http_response = request.GetHttpResponse();
  const auto request_data = ParseRequest(request_json);
  if (request_data.configs.IsEmpty() || request_data.service.empty()) {
    http_response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    return uservice_dynconf::utils::MakeError(
        "400", "Fields 'configs' and 'service' are required");
  }

  cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                    uservice_dynconf::sql::kInsertConfigValue.data(),
                    request_data.service, request_data.configs);

  http_response.SetStatus(userver::server::http::HttpStatus::kNoContent);

  return {};
}

} // namespace uservice_dynconf::handlers::admin_v1_configs::post
