#include "view.hpp"
#include "cache/configs_cache.hpp"
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
  std::int32_t kLimit = 50;
  std::int32_t kOffset = 0;
  if (request.HasHeader(OFFSET)) {
    try {
      kOffset = stoi(request.GetHeader(OFFSET));
    } catch (...) {
      auto &response = request.GetHttpResponse();
      response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
      return {};
    }
  }
  if (request.HasHeader(LIMIT)) {
    try {
      kLimit = stoi(request.GetHeader(LIMIT));
    } catch (...) {
      auto &response = request.GetHttpResponse();
      response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
      return {};
    }
  }
  if (kOffset < 0 || kLimit < 0) {
    auto &response = request.GetHttpResponse();
    response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    return {};
  }

  auto result = pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      uservice_dynconf::sql::kSelectConfigs.data());

  userver::formats::json::ValueBuilder response;
  response["items"].Resize(0);
  std::int32_t count = 0;
  for (auto row = result.AsSetOf<std::string>().begin() +
                  std::min(kOffset, (int32_t)result.Size());
       row < result.AsSetOf<std::string>().end(); ++row) {
    if (count >= kLimit)
      break;
    response["items"].PushBack(*row);
    count++;
  }
  response["count"] = count;
  response["total"] = result.Size();

  return userver::formats::json::ToString(response.ExtractValue());
}

} // namespace uservice_dynconf::handlers::configs::get
