#include "configs_values.hpp"
#include "cache/configs_cache.hpp"
#include "userver/formats/json/inline.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/utils/datetime.hpp"
#include <chrono>
#include <ctime>

namespace uservice_dynconf::handlers::configs_values::post {

namespace {
struct RequestData {
  std::vector<std::string> ids{};
  std::optional<std::chrono::time_point<std::chrono::system_clock>>
      update_since{};
  std::string service{};
};

RequestData ParseRequest(const userver::formats::json::Value &request) {
  RequestData result;
  result.ids = request["ids"].As<std::vector<std::string>>({});
  result.service = request["service"].As<std::string>({});
  if (auto str_time = request["updated_since"].As<std::string>({});
      !str_time.empty()) {
    result.update_since = {userver::utils::datetime::Stringtime(
        str_time, userver::utils::datetime::kDefaultTimezone,
        userver::utils::datetime::kRfc3339Format)};
  }
  return result;
}
} // namespace

Handler::Handler(const userver::components::ComponentConfig &config,
                 const userver::components::ComponentContext &context)
    : HttpHandlerJsonBase(config, context),
      cache_(context.FindComponent<
             uservice_dynconf::cache::settings_cache::ConfigsCache>()) {}

userver::formats::json::Value Handler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest &,
    const userver::formats::json::Value &request_json,
    userver::server::request::RequestContext &) const {

  const auto request_data = ParseRequest(request_json);
  const auto data = cache_.Get();

  userver::formats::json::ValueBuilder result =
      userver::formats::json::MakeObject();

  constexpr std::chrono::time_point<std::chrono::system_clock> kMinTime(
      std::chrono::milliseconds(0));
  std::chrono::time_point<std::chrono::system_clock> updated_at(
      std::chrono::milliseconds(0));

  const auto configs =
      request_data.ids.empty()
          ? data->FindConfigsByService(request_data.service)
          : data->FindConfigs(request_data.service, request_data.ids);

  for (const auto &config : configs) {
    if (config && request_data.update_since.value_or(kMinTime) <=
                      config->updated_at.GetUnderlying()) {
      result[config->key.config_name] = config->config_value;
      updated_at = std::max(updated_at, config->updated_at.GetUnderlying());
    }
  }

  userver::formats::json::ValueBuilder builder;
  builder["configs"] = result.ExtractValue();
  builder["updated_at"] =
      updated_at == kMinTime ? userver::utils::datetime::Now() : updated_at;
  return builder.ExtractValue();
}

} // namespace uservice_dynconf::handlers::configs_values::post
