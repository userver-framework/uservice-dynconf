#pragma once

#include "userver/formats/json/value.hpp"
#include <string_view>
namespace service_dynamic_configs::utils {

userver::formats::json::Value MakeError(std::string_view code,
                                        std::string_view message);

}
