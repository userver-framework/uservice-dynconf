#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/utils/daemon_run.hpp>
#include <userver/server/handlers/http_handler_static.hpp>

#include "cache/configs/configs_cache.hpp"
#include "cache/services/services_cache.hpp"

#include "handlers/admin/v1/post-configs/view.hpp"
#include "handlers/admin/v1/post-configs_delete/view.hpp"
#include "handlers/post-configs_values/view.hpp"

#include "handlers/admin/v2/delete-configs_uuid/view.hpp"
#include "handlers/admin/v2/get-configs/view.hpp"
#include "handlers/admin/v2/get-configs_uuid/view.hpp"
#include "handlers/admin/v2/get-services/view.hpp"
#include "handlers/admin/v2/patch-configs_uuid/view.hpp"
#include "handlers/admin/v2/post-configs/view.hpp"
#include "handlers/admin/v2/post-configs_uuid_clone/view.hpp"
#include "handlers/admin/v2/post-services_uuid_clone/view.hpp"

#include "handlers/admin/options/view.hpp"

#include "userver/clients/dns/component.hpp"
#include "userver/clients/http/component.hpp"
#include "userver/testsuite/testsuite_support.hpp"
#include <userver/server/component.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/storages/postgres/component.hpp>

int main(int argc, char *argv[]) {
  namespace service_handlers = uservice_dynconf::handlers;
  auto component_list =
      userver::components::MinimalServerComponentList()
          .Append<userver::server::handlers::Ping>()
          .Append<userver::components::Postgres>("settings-database")
          .Append<userver::clients::dns::Component>()
          .Append<userver::components::TestsuiteSupport>()
          .Append<uservice_dynconf::cache::settings_cache::ConfigsCache>()
          .Append<uservice_dynconf::cache::settings_cache::ServicesCache>()
          .Append<service_handlers::configs_values::post::Handler>()
          .Append<service_handlers::admin_v1_configs::post::Handler>()
          .Append<service_handlers::admin_v1_configs_delete::post::Handler>()
          .Append<service_handlers::configs_uuid::get::Handler>()
          .Append<service_handlers::configs_uuid::patch::Handler>()
          .Append<service_handlers::configs::get::Handler>()
          .Append<service_handlers::services::get::Handler>()
          .Append<service_handlers::configs_uuid::del::Handler>()
          .Append<service_handlers::configs::post::Handler>()
          .Append<service_handlers::configs_uuid_clone::post::Handler>()
          .Append<service_handlers::services_uuid_clone::post::Handler>()
          .Append<service_handlers::options::Handler>()
          .Append<userver::components::HttpClient>()
          .Append<userver::server::handlers::TestsControl>()
          .Append<userver::components::FsCache>("fs-cache-main")
          .Append<userver::server::handlers::HttpHandlerStatic>();

  return userver::utils::DaemonMain(argc, argv, component_list);
}
