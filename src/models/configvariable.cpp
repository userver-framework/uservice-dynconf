#include "configvariable.hpp"

namespace uservice_dynconf::models {
    userver::formats::json::Value Serialize(const ConfigVariable& response,
                                            userver::formats::serialize::To<userver::formats::json::Value>) {
        userver::formats::json::ValueBuilder item;
        item["uuid"] = response.uuid;
        item["service"] = response.service;
        item["config_name"] = response.config_name;
        item["config_value"] = response.config_value;
        return item.ExtractValue();
    }
}  // namespace bookmarker