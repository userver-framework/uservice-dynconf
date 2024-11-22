#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/component.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>

#include <cache/configs_cache.hpp>
#include <handlers/admin_v1_configs.hpp>
#include <handlers/admin_v1_configs_delete.hpp>
#include <handlers/configs_values.hpp>

int main(int argc, char *argv[]) {
  namespace service_handlers = uservice_dynconf::handlers;
  auto component_list =
      userver::components::MinimalServerComponentList()
          .Append<userver::server::handlers::Ping>()
          .Append<userver::components::Postgres>("settings-database")
          .Append<userver::clients::dns::Component>()
          .Append<userver::components::TestsuiteSupport>()
          .Append<uservice_dynconf::cache::settings_cache::ConfigsCache>()
          .Append<service_handlers::configs_values::post::Handler>()
          .Append<service_handlers::admin_v1_configs::post::Handler>()
          .Append<service_handlers::admin_v1_configs_delete::post::Handler>()
          .Append<userver::components::HttpClient>()
          .Append<userver::server::handlers::TestsControl>();
  return userver::utils::DaemonMain(argc, argv, component_list);
}
