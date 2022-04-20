#pragma once

#include "userver/formats/json/value.hpp"
#include <string_view>
namespace uservice_dynconf::utils {

userver::formats::json::Value MakeError(std::string_view code,
                                        std::string_view message);

}
