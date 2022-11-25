#pragma once
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/server/handlers/http_handler_base.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "userver/storages/postgres/postgres_fwd.hpp"
#include <string_view>
#include <userver/components/component_list.hpp>

using namespace userver;

namespace uservice_dynconf::handlers::configs_uuid_clone::post {

class Handler final : public server::handlers::HttpHandlerJsonBase {
public:
  static constexpr std::string_view kName = "handler-configs-uuid-clone";

  Handler(const components::ComponentConfig &config,
          const components::ComponentContext &component_context);

  std::optional<std::string>
  GetServiceUuid(const std::string &uuid,
                 const std::optional<std::string> &service_name) const;

  formats::json::Value HandleRequestJsonThrow(
      const server::http::HttpRequest &request,
      const formats::json::Value &json,
      server::request::RequestContext &) const override final;

private:
  struct RequestData {
    std::string config_name;
    std::optional<std::string> config_value;
  };
  const storages::postgres::ClusterPtr cluster_;
};
} // namespace uservice_dynconf::handlers::configs_uuid_clone::post
