-- See https://userver.tech/de/d10/md_en_2schemas_2dynamic__configs.html
-- for description of dynamic configs.

INSERT INTO uservice_dynconf.configs (config_name, config_value)
VALUES ('HTTP_CLIENT_CONNECT_THROTTLE', '{
  "http-limit": 6000,
  "http-per-second": 1500,
  "https-limit": 100,
  "https-per-second": 25,
  "per-host-limit": 3000,
  "per-host-per-second": 500
}'),
('HTTP_CLIENT_CONNECTION_POOL_SIZE', '5000'),
('MONGO_DEFAULT_MAX_TIME_MS', '200'),
('POSTGRES_CONNECTION_SETTINGS', '{}'),
('POSTGRES_CONGESTION_CONTROL_SETTINGS', '{}'),
('POSTGRES_CONNLIMIT_MODE_AUTO_ENABLED', 'false'),
('POSTGRES_DEFAULT_COMMAND_CONTROL', '{
  "network_timeout_ms": 750,
  "statement_timeout_ms": 500
}'),
('POSTGRES_HANDLERS_COMMAND_CONTROL', '{}'),
('POSTGRES_QUERIES_COMMAND_CONTROL', '{}'),
('POSTGRES_CONNECTION_POOL_SETTINGS', '{
  "__default__": {
    "min_pool_size": 4,
    "max_pool_size": 15,
    "max_queue_size": 200
  }
}'),
('POSTGRES_STATEMENT_METRICS_SETTINGS','{}'),
('POSTGRES_CONNECTION_PIPELINE_EXPERIMENT','1'),
('POSTGRES_DEADLINE_PROPAGATION_VERSION','1'),
('USERVER_CACHES', '{}'),
('USERVER_CHECK_AUTH_IN_HANDLERS', 'false'),
('USERVER_CANCEL_HANDLE_REQUEST_BY_DEADLINE', 'false'),
('USERVER_DEADLINE_PROPAGATION_ENABLED', 'true'),
('USERVER_DUMPS', '{}'),
('USERVER_HTTP_PROXY', '""'),
('USERVER_HANDLER_STREAM_API_ENABLED', 'false'),
('USERVER_LOG_REQUEST', 'true'),
('USERVER_LOG_REQUEST_HEADERS', 'false'),
('USERVER_LRU_CACHES', '{}'),
('USERVER_RPS_CCONTROL_CUSTOM_STATUS', '{}'),
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
('USERVER_TASK_PROCESSOR_PROFILER_DEBUG', '{}'),
('BAGGAGE_SETTINGS', '{"allowed_keys": []}'),
('USERVER_BAGGAGE_ENABLED', 'false'),
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
