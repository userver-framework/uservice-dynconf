#include "view.hpp"
#include "cache/configs_cache.hpp"
#include "models/variablewithdate.hpp"
#include "sql/sql_query.hpp"
#include "userver/formats/json/inline.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/storages/postgres/cluster.hpp"
#include "userver/storages/postgres/component.hpp"
#include "userver/utils/datetime.hpp"
#include <chrono>
#include <ctime>

namespace uservice_dynconf::handlers::variables::get {
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

  std::int32_t kLimit = 50;
  std::int32_t kOffset = 0;
  if (request.HasArg(OFFSET)) {
    try {
      kOffset = stoi(request.GetArg(OFFSET));
    } catch (...) {
      http_response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
      return {};
    }
  }
  if (request.HasArg(LIMIT)) {
    try {
      kLimit = stoi(request.GetArg(LIMIT));
    } catch (...) {
      http_response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
      return {};
    }
  }
  if (kOffset < 0 || kLimit < 0) {
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
                      .AsSetOf<uservice_dynconf::models::VariableWithDate>(
                          userver::storages::postgres::kRowTag)
                      .begin() +
                  std::min(kOffset, (int32_t)result.Size());
       row < result
                 .AsSetOf<uservice_dynconf::models::VariableWithDate>(
                     userver::storages::postgres::kRowTag)
                 .end();
       ++row) {
    if (count >= kLimit)
      break;
    response["items"].PushBack(*row);
    count++;
  }
  response["count"] = count;
  response["total"] = result.Size();

  return userver::formats::json::ToString(response.ExtractValue());
}

} // namespace uservice_dynconf::handlers::variables::get