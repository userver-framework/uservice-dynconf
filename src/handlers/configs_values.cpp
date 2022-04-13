#include "configs_values.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/utils/datetime.hpp"
#include <chrono>

namespace service_dynamic_configs::handlers::configs_values::post {

namespace {
struct RequestData {
  std::vector<std::string> ids{};
  std::optional<std::chrono::time_point<std::chrono::system_clock>>
      update_since{};
  std::string service{};
};

RequestData ParseRequest(const userver::formats::json::Value &request) {
  RequestData result;
  if (request["ids"].IsArray()) {
    result.ids = request["ids"].As<std::vector<std::string>>();
  }
  if (request["service"].IsString()) {
    result.service = request["service"].As<std::string>();
  }
  if (request["updated_since"].IsString()) {
    if (auto str_time = request["updated_since"].As<std::string>();
        !str_time.empty()) {
      result.update_since = {userver::utils::datetime::Stringtime(
          str_time, userver::utils::datetime::kDefaultTimezone,
          userver::utils::datetime::kRfc3339Format)};
    }
  }
  return result;
}
} // namespace

Handler::Handler(const userver::components::ComponentConfig &config,
                 const userver::components::ComponentContext &context)
    : HttpHandlerJsonBase(config, context),
      cache_(context.FindComponent<
             service_synamic_configs::cache::settings_cache::ConfigsCache>()) {}

userver::formats::json::Value Handler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest &,
    const userver::formats::json::Value &request_json,
    userver::server::request::RequestContext &) const {

  const auto request_data = ParseRequest(request_json);
  const auto data = cache_.Get();

  userver::formats::json::ValueBuilder result;

  std::optional<std::chrono::time_point<std::chrono::system_clock>> updated_at;

  if (!request_data.ids.empty()) {
    for (const auto &id : request_data.ids) {
      if (const auto val = data->FindConfig({request_data.service, id});
          val &&
          (!request_data.update_since || request_data.update_since.value() <=
                                             val->updated_at.GetUnderlying())) {
        result[val->key.config_name] = val->config_value;
        updated_at =
            updated_at ? std::max(*updated_at, val->updated_at.GetUnderlying())
                       : val->updated_at.GetUnderlying();
      }
    }
  } else {
    auto confs = data->FindConfigsByService(request_data.service);
    for (const auto &val : confs) {
      if (val &&
          (!request_data.update_since || request_data.update_since.value() <=
                                             val->updated_at.GetUnderlying())) {
        result[val->key.config_name] = val->config_value;
        updated_at =
            updated_at ? std::max(*updated_at, val->updated_at.GetUnderlying())
                       : val->updated_at.GetUnderlying();
      }
    }
  }

  userver::formats::json::ValueBuilder builder =
      userver::formats::json::MakeObject();
  builder["configs"] = result.ExtractValue();
  builder["updated_at"] = updated_at.value_or(userver::utils::datetime::Now());
  return builder.ExtractValue();
}

} // namespace service_dynamic_configs::handlers::configs_values::post
