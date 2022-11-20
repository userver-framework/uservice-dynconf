#pragma once

#include "cache/configs/configs_cache.hpp"
#include "userver/storages/postgres/query.hpp"
#include "userver/components/component_context.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/server/handlers/http_handler_base.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include <string_view>

namespace uservice_dynconf::handlers::configs::get {

class Handler final : public userver::server::handlers::HttpHandlerBase {
public:
  static constexpr std::string_view kName = "handler-get-configs";

  Handler(const userver::components::ComponentConfig &config,
          const userver::components::ComponentContext &context);

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest &request,
      userver::server::request::RequestContext &context) const override final;

private:
  userver::storages::postgres::ClusterPtr pg_cluster_;
  const std::string LIMIT = "limit";
  const std::string PAGE = "page";
  const std::string S = "s";
  const std::string S_SERVICES = "s_services";
};
} // namespace uservice_dynconf::handlers::variables::get