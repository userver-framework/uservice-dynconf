#include "admin_v1_configs.hpp"
#include "userver/formats/json/inline.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/formats/yaml/value_builder.hpp"
#include "userver/storages/postgres/cluster.hpp"
#include "userver/storages/postgres/component.hpp"

#include "utils/make_error.hpp"
#include "sql/sql_query.hpp"
#include <vector>

namespace service_dynamic_configs::handlers::admin_v1_configs::post {

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
    return service_dynamic_configs::utils::MakeError("400", "Fields configs and service requred");
  }

  auto trx = cluster_->Begin(
      "add_and_update_configs",
      userver::storages::postgres::ClusterHostType::kMaster, {});

  for (const auto &[k, v] :
       userver::formats::json::Items(request_data.configs)) {
    trx.Execute(service_dynamic_configs::sql::kInsertConfigValue.data(),
                request_data.service, k, v);
  }
  trx.Commit();
  http_response.SetStatus(userver::server::http::HttpStatus::kNoContent);

  return {};
}

} // namespace service_dynamic_configs::handlers::admin_v1_configs::post
