#include "view.hpp"
#include "sql/sql_query.hpp"
#include "models/get_variables_responce.hpp"
#include "cache/configs_cache.hpp"
#include "userver/formats/json/inline.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/storages/postgres/cluster.hpp"
#include "userver/storages/postgres/component.hpp"
#include "userver/utils/datetime.hpp"
#include <chrono>
#include <ctime>

namespace uservice_dynconf::handlers::get_variables::get {
    Handler::Handler(const userver::components::ComponentConfig &config,
                     const userver::components::ComponentContext &context)
            : HttpHandlerBase(config, context),
              pg_cluster_(
                      context
                              .FindComponent<userver::components::Postgres>("settings-database")
                              .GetCluster()) {}

    std::string Handler::HandleRequestThrow(
            const userver::server::http::HttpRequest &,
            userver::server::request::RequestContext &) const {
        auto result = pg_cluster_->Execute(
                userver::storages::postgres::ClusterHostType::kMaster,
                uservice_dynconf::sql::kSelectAll.data()
        );

        userver::formats::json::ValueBuilder response;
        response["items"].Resize(0);
        response["offset"] = result.Size();
        response["limit"] = kMaxReturnCount;
        std::int32_t i = 0;
        for (auto row : result.AsSetOf<uservice_dynconf::models::TResponse>(userver::storages::postgres::kRowTag)) {
            response["items"].PushBack(row);
            i++;
            if(i > kMaxReturnCount)
                break;
        }

        return userver::formats::json::ToString(response.ExtractValue());
    }

} // namespace uservice_dynconf::handlers::get-variables::get