#include <handlers/admin_v1_configs.hpp>

#include <string>
#include <unordered_set>

#include <userver/formats/json/inline.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/formats/yaml/value_builder.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

#include <docs/api/api.hpp>
#include <uservice_dynconf/sql_queries.hpp>

#include <models/config.hpp>
#include <utils/make_error.hpp>

namespace uservice_dynconf::handlers::admin_v1_configs::post {

namespace {

bool ConsistsOfIdsFromConfigs(
    const std::unordered_set<std::string> &kill_switches,
    const userver::formats::json::Value &configs) {
  for (const auto &kill_switch : kill_switches) {
    if (!configs.HasMember(kill_switch)) {
      return false;
    }
  }
  return true;
}

bool HasIntersection(const std::unordered_set<std::string> &first,
                     const std::unordered_set<std::string> &second) {
  for (const auto &key : first) {
    if (second.count(key) > 0) {
      return true;
    }
  }
  return false;
}

userver::formats::json::Value MakeConfigModeMap(
    const userver::formats::json::Value &configs,
    const std::unordered_set<std::string> &kill_switches_enabled,
    const std::unordered_set<std::string> &kill_switches_disabled) {
  using Mode = uservice_dynconf::models::Mode;

  userver::formats::json::ValueBuilder builder;
  for (const auto &[config_name, config_value] : Items(configs)) {
    Mode config_mode = Mode::kDynamicConfig;
    if (kill_switches_enabled.count(config_name) > 0) {
      config_mode = Mode::kKillSwitchEnabled;
    } else if (kill_switches_disabled.count(config_name) > 0) {
      config_mode = Mode::kKillSwitchDisabled;
    }
    builder[config_name] = uservice_dynconf::models::ToString(config_mode);
  }

  return builder.ExtractValue();
}

} // namespace

Handler::Handler(const userver::components::ComponentConfig &config,
                 const userver::components::ComponentContext &context)
    : HttpHandlerJsonBase(config, context),
      cluster_(
          context
              .FindComponent<userver::components::Postgres>("settings-database")
              .GetCluster()) {}

userver::formats::json::Value Handler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest &request,
    const userver::formats::json::Value &request_json,
    userver::server::request::RequestContext &) const {
  auto &http_response = request.GetHttpResponse();
  auto &&request_data = request_json.As<AdminConfigsRequestBody>();

  if (!request_data.configs.has_value() ||
      request_data.configs.value().extra.IsEmpty() ||
      !request_data.service.has_value() ||
      request_data.service.value().empty()) {
    http_response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    return uservice_dynconf::utils::MakeError(
        "400", "Fields 'configs' and 'service' are required");
  }

  const auto configs = request_data.configs.value().extra;
  const auto kill_switches_enabled =
      request_data.kill_switches_enabled.value_or(
          std::unordered_set<std::string>{});
  const auto kill_switches_disabled =
      request_data.kill_switches_disabled.value_or(
          std::unordered_set<std::string>{});

  if (!ConsistsOfIdsFromConfigs(kill_switches_enabled, configs) ||
      !ConsistsOfIdsFromConfigs(kill_switches_disabled, configs)) {
    http_response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    return uservice_dynconf::utils::MakeError(
        "400", "Fields 'kill_switches_enabled' and 'kill_switches_disabled' "
               "must consist of ids from 'configs' field");
  }
  if (HasIntersection(kill_switches_enabled, kill_switches_disabled)) {
    http_response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    return uservice_dynconf::utils::MakeError(
        "400", "Ids in 'kill_switches_enabled' and 'kill_switches_disabled' "
               "must not overlap");
  }

  const auto config_mode_map =
      MakeConfigModeMap(configs, kill_switches_enabled, kill_switches_disabled);
  cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                    uservice_dynconf::sql::kInsertConfigValue,
                    request_data.service.value(), configs, config_mode_map);

  http_response.SetStatus(userver::server::http::HttpStatus::kNoContent);

  return {};
}

} // namespace uservice_dynconf::handlers::admin_v1_configs::post
