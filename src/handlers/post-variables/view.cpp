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

namespace uservice_dynconf::handlers::variables::post {
namespace {
struct RequestData {
  std::string config_value{};
  std::string config_name{};
  std::string service_name{};
  bool isValid() const {
    return !config_value.empty() && !config_name.empty() &&
           !service_name.empty();
  }
};
RequestData ParseRequest(const userver::formats::json::Value &request) {
  RequestData result;
  result.config_value = request["value"].As<std::string>({});
  result.config_name = request["name"].As<std::string>({});
  result.service_name = request["service"].As<std::string>({});
  return result;
}
class Handler final : public userver::server::handlers::HttpHandlerJsonBase {
public:
  static constexpr std::string_view kName = "handler-variables-post";

  Handler(const userver::components::ComponentConfig &config,
          const userver::components::ComponentContext &component_context)
      : HttpHandlerJsonBase(config, component_context),
        cluster_(component_context
                     .FindComponent<userver::components::Postgres>(
                         "settings-database")
                     .GetCluster()) {}

  userver::formats::json::Value HandleRequestJsonThrow(
      const userver::server::http::HttpRequest &request,
      const userver::formats::json::Value &json,
      userver::server::request::RequestContext &) const override {
    auto &http_response = request.GetHttpResponse();
    const auto request_data = ParseRequest(json);
    try {
      auto val =
          userver::formats::json::FromString(json["value"].As<std::string>({}));
    } catch (const userver::formats::json::ParseException &e) {
      http_response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
      return uservice_dynconf::utils::MakeError("400", e.what());
    }
    if (!request_data.isValid()) {
      http_response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
      return uservice_dynconf::utils::MakeError(
          "400", "Fields 'name' , 'value' and 'service' are required");
    }
    auto result = cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        uservice_dynconf::sql::kInsertConfigVariableValue.data(),
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

private:
  const userver::storages::postgres::ClusterPtr cluster_;
};
} // namespace
void AppendVariableHandler(userver::components::ComponentList &component_list) {
  component_list.Append<Handler>();
}
} // namespace uservice_dynconf::handlers::variables::post
