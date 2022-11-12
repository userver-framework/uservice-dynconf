#include "responce.hpp"

namespace uservice_dynconf::models {

    userver::formats::json::Value Serialize(const TResponse& response,
                                            userver::formats::serialize::To<userver::formats::json::Value>) {
        userver::formats::json::ValueBuilder item;
        item["uuid"] = bookmark.uuid;
        item["service"] = bookmark.service;
        item["config_name"] = bookmark.config_name;
        item["updated_at"] = bookmark.updated_at;
        return item.ExtractValue();
    }

}
