#pragma once

#include <chrono>
#include <string>

#include <userver/formats/json/value_builder.hpp>

namespace uservice_dynconf::models {
    struct TResponse {
        std::string uuid;
        std::string service;
        std::string config_name;
        std::chrono::system_clock::time_point updated_at;
    };

    userver::formats::json::Value Serialize(const TResponse& data,
                                            userver::formats::serialize::To<userver::formats::json::Value>);

}  // namespace bookmarker
