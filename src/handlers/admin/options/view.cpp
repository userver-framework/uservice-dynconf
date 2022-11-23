#include "view.hpp"

#include <userver/clients/http/client.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/components/component_context.hpp>
#include <userver/logging/log.hpp>
#include <userver/utils/assert.hpp>

namespace uservice_dynconf::handlers::options {
Handler::Handler(const userver::components::ComponentConfig &config,
                 const userver::components::ComponentContext &context)
    : HttpHandlerBase(config, context) {}

std::string
Handler::HandleRequestThrow(const userver::server::http::HttpRequest &request,
                            userver::server::request::RequestContext &) const {
  auto &response = request.GetHttpResponse();
  response.SetHeader("Access-Control-Allow-Origin", "*");
  response.SetHeader("Access-Control-Allow-Methods",
                     "GET, POST, OPTIONS, PATCH, DELETE");
  response.SetHeader("Access-Control-Allow-Headers",
                     "DNT,User-Agent,X-Requested-With,If-Modified-Since,Cache-"
                     "Control,Content-Type,Range,Ticket,User");
  response.SetHeader("Access-Control-Max-Age", "1728000");
  response.SetStatus(userver::server::http::HttpStatus::kNoContent);
  return {};
}

} // namespace uservice_dynconf::handlers::options