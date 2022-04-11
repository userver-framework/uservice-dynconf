#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/utils/daemon_run.hpp>

#include "cache/settings_cache.hpp"
#include "handlers/configs_values.hpp"
#include "userver/clients/dns/component.hpp"
#include "userver/testsuite/testsuite_support.hpp"

#include <userver/storages/postgres/component.hpp>

#include "components/pg_init.hpp"

int main(int argc, char *argv[]) {
  auto component_list =
      userver::components::MinimalServerComponentList()
          .Append<userver::server::handlers::Ping>()
          .Append<userver::components::Postgres>("settings-database")
          .Append<userver::clients::dns::Component>()
          .Append<userver::components::TestsuiteSupport>()
          .Append<service_dynamic_configs::components::pg_init::Component>()
          .Append<
              service_synamic_configs::cache::settings_cache::ConfigsCache>()
          .Append<service_dynamic_configs::handlers::configs_values::post::
                      Handler>();
  return userver::utils::DaemonMain(argc, argv, component_list);
}
