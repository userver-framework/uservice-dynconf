#pragma once
#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"
#include "userver/formats/json/value.hpp"
#include "userver/server/handlers/http_handler_base.hpp"
#include "userver/server/handlers/http_handler_json_base.hpp"
#include "userver/storages/postgres/postgres_fwd.hpp"
#include <string_view>
#include <userver/components/component_list.hpp>

namespace uservice_dynconf::handlers::configs::post {
void AppendVariableHandler(userver::components::ComponentList &component_list);
}
