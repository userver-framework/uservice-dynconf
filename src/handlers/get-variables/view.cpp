#include "view.hpp"
#include "cache/configs_cache.hpp"
#include "sql/sql_query.hpp"
#include "userver/formats/json/inline.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/formats/serialize/common_containers.hpp"
#include "userver/storages/postgres/cluster.hpp"
#include "userver/storages/postgres/component.hpp"
#include "userver/utils/datetime.hpp"
#include <chrono>
#include <ctime>

namespace uservice_dynconf::handlers::variables::get {

struct RequestData {
  std::string uuid;
  std::string service;
  std::string config_name;
  std::optional<std::string> value;
  std::chrono::system_clock::time_point updated_at;
};

userver::formats::json::Value
Serialize(const RequestData &response,
          userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder item;
  item["uuid"] = response.uuid;
  item["service"] = response.service;
  item["config_name"] = response.config_name;
  if (response.value.has_value())
    item["value"] = response.value.value();
  else
    item["config_value"] = "null";
  item["updated_at"] = response.updated_at;
  return item.ExtractValue();
}

Handler::Handler(const userver::components::ComponentConfig &config,
                 const userver::components::ComponentContext &context)
    : HttpHandlerBase(config, context),
      pg_cluster_(
          context
              .FindComponent<userver::components::Postgres>("settings-database")
              .GetCluster()) {}

std::string
Handler::HandleRequestThrow(const userver::server::http::HttpRequest &request,
                            userver::server::request::RequestContext &) const {
  auto &http_response = request.GetHttpResponse();
  http_response.SetHeader("Access-Control-Allow-Origin", "*");

  std::int32_t limit = 50;
  std::int32_t page = 1;
  std::string s;
  if (request.HasArg(PAGE)) {
    try {
      page = stoi(request.GetArg(PAGE));
    } catch (...) {
      http_response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
      return {};
    }
  }
  if (request.HasArg(LIMIT)) {
    try {
      limit = stoi(request.GetArg(LIMIT));
    } catch (...) {
      http_response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
      return {};
    }
  }
  if (request.HasArg(S)) {
    s = request.GetArg(S);
  }

  if (page <= 0 || limit <= 0) {
    http_response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    return {};
  }

  auto result = pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      uservice_dynconf::sql::kSelectAll.data(), s);

  userver::formats::json::ValueBuilder response;
  response["items"].Resize(0);
  for (auto row =
           result.AsSetOf<RequestData>(userver::storages::postgres::kRowTag)
               .begin() +
           std::min((page - 1) * limit, (int32_t)result.Size());
       row < result.AsSetOf<RequestData>(userver::storages::postgres::kRowTag)
                     .begin() +
                 std::min((page)*limit, (int32_t)result.Size());
       ++row) {
    response["items"].PushBack(*row);
  }
  response["total"] = result.Size();
  http_response.SetHeader("Content-Type", "application/json");
  return userver::formats::json::ToString(response.ExtractValue());
}

} // namespace uservice_dynconf::handlers::variables::get
