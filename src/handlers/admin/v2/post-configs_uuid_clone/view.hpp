#pragma once
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/server/handlers/http_handler_base.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "userver/storages/postgres/postgres_fwd.hpp"
#include <string_view>
#include <userver/components/component_list.hpp>

namespace uservice_dynconf::handlers::configs_uuid_clone::post {

class Handler final : public userver::server::handlers::HttpHandlerJsonBase {
public:
  static constexpr std::string_view kName = "handler-configs-uuid-clone";

  Handler(const userver::components::ComponentConfig &config,
          const userver::components::ComponentContext &component_context);

  userver::formats::json::Value HandleRequestJsonThrow(
      const userver::server::http::HttpRequest &request,
      const userver::formats::json::Value &json,
      userver::server::request::RequestContext &) const override final;

private:
  const userver::storages::postgres::ClusterPtr cluster_;
};
} // namespace uservice_dynconf::handlers::config_uuid_clone::post
