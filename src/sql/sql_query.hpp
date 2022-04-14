#pragma once

namespace service_dynamic_configs::sql {

inline constexpr std::string_view kCreateSchema = R"~(
CREATE SCHEMA IF NOT EXISTS service_dynamic_configs;
)~";

inline constexpr std::string_view kCreateTable = R"~(
CREATE TABLE IF NOT EXISTS service_dynamic_configs.configs (
    service TEXT NOT NULL DEFAULT '__default__',
    config_name TEXT NOT NULL,
    config_value JSONB,
    created_at timestamptz DEFAULT NOW(),
    updated_at timestamptz DEFAULT NOW(),

    PRIMARY KEY (service, config_name)
);
)~";

inline constexpr std::string_view kCreateIndexCreated = R"~(
CREATE INDEX IF NOT EXISTS idx__created_at__configs
ON service_dynamic_configs.configs USING btree (created_at);
)~";

inline constexpr std::string_view kCreateIndexUpdate = R"~(
CREATE INDEX IF NOT EXISTS idx__updated_at__configs
ON service_dynamic_configs.configs USING btree (updated_at);
)~";

inline constexpr std::string_view kCreateIndexPair = R"~(
CREATE UNIQUE INDEX IF NOT EXISTS idx__pair_service_and_connfig
ON service_dynamic_configs.configs USING btree (service, config_name);
)~";

inline constexpr std::string_view kInsertDefaultConfigs = R"~(
INSERT INTO service_dynamic_configs.configs (config_name, config_value)
VALUES ('HTTP_CLIENT_CONNECT_THROTTLE', '{
  "http-limit": 6000,
  "http-per-second": 1500,
  "https-limit": 100,
  "https-per-second": 25,
  "per-host-limit": 3000,
  "per-host-per-second": 500
}'),
       ('HTTP_CLIENT_CONNECTION_POOL_SIZE',
        '5000'),
        ('HTTP_CLIENT_ENFORCE_TASK_DEADLINE',
        '{
  "cancel-request": false,
  "update-timeout": false
}'),
('MONGO_DEFAULT_MAX_TIME_MS', '200'),
('POSTGRES_DEFAULT_COMMAND_CONTROL',
'{
  "network_timeout_ms": 750,
  "statement_timeout_ms": 500
}'),
('POSTGRES_HANDLERS_COMMAND_CONTROL',
'{}'),
('POSTGRES_QUERIES_COMMAND_CONTROL', '
{
  "cleanup_processed_data": {
    "network_timeout_ms": 92000,
    "statement_timeout_ms": 90000
  },
  "select_recent_users": {
    "network_timeout_ms": 70,
    "statement_timeout_ms": 30
  }
}'),
('POSTGRES_CONNECTION_POOL_SETTINGS', '
{
  "__default__": {
    "min_pool_size": 4,
    "max_pool_size": 15,
    "max_queue_size": 200
  }
}
'),
('POSTGRES_STATEMENT_METRICS_SETTINGS','{}'),
('USERVER_CACHES', '{}'),
('USERVER_CHECK_AUTH_IN_HANDLERS', 'false'),
('USERVER_CANCEL_HANDLE_REQUEST_BY_DEADLINE', 'false'),
('USERVER_DUMPS', '{}'),
('USERVER_HTTP_PROXY', '{}'),
('USERVER_LOG_REQUEST', 'true'),
('USERVER_LOG_REQUEST_HEADERS', 'false'),
('USERVER_LRU_CACHES', '{}'),
('USERVER_NO_LOG_SPANS', '{"names": [], "prefixes": []}'),
('USERVER_RPS_CCONTROL', '{
  "down-level": 8,
  "down-rate-percent": 1,
  "load-limit-crit-percent": 70,
  "min-limit": 2,
  "no-limit-seconds": 120,
  "overload-off-seconds": 8,
  "overload-on-seconds": 8,
  "up-level": 2,
  "up-rate-percent": 1
}'),
('USERVER_RPS_CCONTROL_ENABLED', 'false'),
('USERVER_TASK_PROCESSOR_PROFILER_DEBUG', '{
  "fs-task-processor": {
    "enabled": false,
    "execution-slice-threshold-us": 1000000
  },
  "main-task-processor": {
    "enabled": false,
    "execution-slice-threshold-us": 2000
  }
}'),
  ('USERVER_TASK_PROCESSOR_QOS', '{
   "default-service": {
    "default-task-processor": {
      "wait_queue_overload": {
        "action": "cancel",
        "length_limit": 5000,
        "sensor_time_limit_us": 12000,
        "time_limit_us": 0
      }
    }
  }
   }')
ON CONFLICT (service, config_name)
DO NOTHING;
)~";

inline constexpr std::string_view kSelectSettingsForCache = R"~(
SELECT (service, config_name), config_value, updated_at
FROM service_dynamic_configs.configs
)~";

inline constexpr std::string_view kInsertConfigValue = R"~(
INSERT INTO service_dynamic_configs.configs
(service, config_name, config_value)
SELECT $1, d.key, d.value::jsonb
FROM jsonb_each_text($2) as d
ON CONFLICT (service, config_name)
DO UPDATE SET
config_value = EXCLUDED.config_value,
updated_at = NOW();
)~";

inline constexpr std::string_view kDeleteConfigValues = R"~(
DELETE FROM service_dynamic_configs.configs
WHERE service = $1 and config_name IN (SELECT unnest($2));
)~";

} // namespace service_dynamic_configs::sql
