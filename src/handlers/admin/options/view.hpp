#pragma once
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"
#include "userver/server/handlers/http_handler_base.hpp"
#include <string_view>
#include <userver/components/component_list.hpp>

namespace uservice_dynconf::handlers::options {
class Handler final : public userver::server::handlers::HttpHandlerBase {
public:
  static constexpr std::string_view kName = "handler-options-mock";

  Handler(const userver::components::ComponentConfig &config,
          const userver::components::ComponentContext &context);

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest &request,
      userver::server::request::RequestContext &context) const override final;
};
} // namespace uservice_dynconf::handlers::options

void AppendOptionsMock(userver::components::ComponentList &component_list);