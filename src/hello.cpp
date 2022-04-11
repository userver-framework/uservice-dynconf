#include "hello.hpp"
#include "cache/settings_cache.hpp"
#include "models/config.hpp"
#include "userver/components/component_context.hpp"
#include "userver/formats/json/inline.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/logging/log.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "userver/storages/postgres/io/chrono.hpp"
#include "userver/utils/datetime.hpp"
#include "userver/utils/strong_typedef.hpp"

#include <chrono>
#include <optional>
#include <string>
#include <userver/server/handlers/http_handler_base.hpp>
#include <vector>

namespace service_template {

namespace {

class Hello final : public userver::server::handlers::HttpHandlerJsonBase {
public:
  static constexpr std::string_view kName = "handler-hello";

  using HttpHandlerJsonBase::HttpHandlerJsonBase;

  Hello(const userver::components::ComponentConfig &config,
        const userver::components::ComponentContext &context)
      : HttpHandlerJsonBase(config, context),
        cache_(context.FindComponent<service_synamic_configs::cache::
                                         settings_cache::ConfigsCache>()) {}

  userver::formats::json::Value HandleRequestJsonThrow(
      const userver::server::http::HttpRequest & /*request*/,
      const userver::formats::json::Value &request_json,
      userver::server::request::RequestContext & /*context*/) const override {

    const auto data = cache_.Get();
    userver::formats::json::ValueBuilder result =
        userver::formats::json::MakeArray();

    if (request_json["ids"].IsArray() && request_json["service"].IsString()) {
      auto ids = request_json["ids"].As<std::vector<std::string>>();
      auto service = request_json["service"].As<std::string>();
      std::optional<std::chrono::time_point<std::chrono::system_clock>> time;
      if (request_json.HasMember("updated_since") &&
          request_json["updated_since"].IsString()) {
        const auto str_time = request_json["updated_since"].As<std::string>();
        time = {userver::utils::datetime::Stringtime(str_time)};
      }

      if (!ids.empty()) {
        for (const auto &id : ids) {
          if (const auto val = data->FindConfig({service, id});
              val &&
              (!time || time.value() <= val->updated_at.GetUnderlying())) {
            result.PushBack(userver::formats::json::MakeObject(
                val->key.config_name, val->config_value));
          }
        }
      } else {
        auto confs = data->FindConfigsByService(service);
        for (const auto &val : confs) {
          if (val &&
              (!time || time.value() <= val->updated_at.GetUnderlying())) {
            result.PushBack(userver::formats::json::MakeObject(
                val->key.config_name, val->config_value));
          }
        }
      }
    }

    userver::formats::json::ValueBuilder builder =
        userver::formats::json::MakeObject();
    builder["configs"] = result.ExtractValue();
    builder["updated_at"] = userver::utils::datetime::Now();

    return builder.ExtractValue();
  }

private:
  const service_synamic_configs::cache::settings_cache::ConfigsCache &cache_;
};

} // namespace

void AppendHello(userver::components::ComponentList &component_list) {
  component_list.Append<Hello>();
}

} // namespace service_template
