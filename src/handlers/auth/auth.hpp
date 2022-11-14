#pragma once

#include "userver/clients/http/client.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/components/component_context.hpp"

#include <userver/clients/http/component.hpp>
#include <userver/components/component.hpp>
#include <userver/components/component_list.hpp>

#include <chrono>
#include <string>

std::string checkSessionToken(std::string token);