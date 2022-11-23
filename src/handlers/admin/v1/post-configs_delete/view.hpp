#pragma once

#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/server/handlers/http_handler_base.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "userver/storages/postgres/postgres_fwd.hpp"
#include <string_view>

namespace uservice_dynconf::handlers::admin_v1_configs_delete::post {

class Handler final : public userver::server::handlers::HttpHandlerJsonBase {
public:
  static constexpr std::string_view kName = "handler-admin-v1-configs-delete";

  Handler(const userver::components::ComponentConfig &config,
          const userver::components::ComponentContext &context);

  userver::formats::json::Value HandleRequestJsonThrow(
      const userver::server::http::HttpRequest &request,
      const userver::formats::json::Value &request_json,
      userver::server::request::RequestContext &context) const override final;

private:
  userver::storages::postgres::ClusterPtr cluster_;
};

} // namespace uservice_dynconf::handlers::admin_v1_configs_delete::post
