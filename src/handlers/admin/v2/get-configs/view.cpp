#include "view.hpp"
#include "userver/storages/postgres/io/chrono.hpp"
#include "sql/sql_query.hpp"
#include "userver/formats/json/inline.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/storages/postgres/cluster.hpp"
#include "userver/storages/postgres/component.hpp"
#include "userver/utils/datetime.hpp"
#include <chrono>
#include <ctime>

namespace uservice_dynconf::handlers::configs::get {
struct DBData {
  std::string uuid;
  std::string service;
  std::string config_name;
  std::chrono::system_clock::time_point updated_at;
};

userver::formats::json::Value Serialize(const DBData& data,
                                        userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder item;
    item["uuid"] = data.uuid;
    item["service"] = data.service;
    item["config_name"] = data.config_name;
    item["updated_at"] = data.updated_at;
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
  std::int32_t offset = 0;
  if (request.HasArg(OFFSET)) {
    try {
      offset = stoi(request.GetArg(OFFSET));
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
  if (offset < 0 || limit < 0) {
    http_response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    return {};
  }

  auto result = pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      uservice_dynconf::sql::kSelectAll.data());

  userver::formats::json::ValueBuilder response;
  response["items"].Resize(0);
  std::int32_t count = 0;
  for (auto row = result
                      .AsSetOf<DBData>(
                          userver::storages::postgres::kRowTag)
                      .begin() +
                  std::min(offset, (int32_t)result.Size());
       row < result
                 .AsSetOf<DBData>(
                     userver::storages::postgres::kRowTag)
                 .end();
       ++row) {
    if (count >= limit)
      break;
    response["items"].PushBack(*row);
    count++;
  }
  response["count"] = count;
  response["total"] = result.Size();

  return userver::formats::json::ToString(response.ExtractValue());
}

} // namespace uservice_dynconf::handlers::variables::get