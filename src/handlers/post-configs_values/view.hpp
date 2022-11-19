#pragma once

#include "cache/configs/configs_cache.hpp"
#include "cache/services/services_cache.hpp"
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/server/handlers/http_handler_base.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include <string_view>

namespace uservice_dynconf::handlers::configs_values::post {

class Handler final : public userver::server::handlers::HttpHandlerJsonBase {
public:
  static constexpr std::string_view kName = "handler-configs-values";

  Handler(const userver::components::ComponentConfig &config,
          const userver::components::ComponentContext &context);

  userver::formats::json::Value HandleRequestJsonThrow(
      const userver::server::http::HttpRequest &request,
      const userver::formats::json::Value &request_json,
      userver::server::request::RequestContext &context) const override final;

private:
  const uservice_dynconf::cache::settings_cache::ConfigsCache &configs_cache_;
  const uservice_dynconf::cache::settings_cache::ServicesCache &services_cache_;
};

} // namespace uservice_dynconf::handlers::configs_values::post
