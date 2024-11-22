#pragma once

#include <string_view>

#include <userver/formats/json/value.hpp>

namespace uservice_dynconf::utils {

userver::formats::json::Value MakeError(std::string_view code,
                                        std::string_view message);

}
