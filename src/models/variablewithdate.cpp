#include "variablewithdate.hpp"

namespace uservice_dynconf::models {
userver::formats::json::Value
Serialize(const VariableWithDate &response,
          userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder item;
  item["uuid"] = response.uuid;
  item["service"] = response.service;
  item["config_name"] = response.config_name;
  item["updated_at"] = response.updated_at;
  return item.ExtractValue();
}
} // namespace uservice_dynconf::models