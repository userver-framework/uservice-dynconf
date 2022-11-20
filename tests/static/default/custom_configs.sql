WITH new_service AS (
  INSERT INTO uservice_dynconf.services (uuid, service_name) VALUES ('__default_uuid__', '__default__')
)
INSERT INTO uservice_dynconf.configs (service_uuid, config_name, config_value)
VALUES 
('__default_uuid__', 'USERVER_RPS_CCONTROL_ENABLED', 'true'),
('__default_uuid__', 'POSTGRES_CONNECTION_POOL_SETTINGS', 
'{
  "__default__": {
    "min_pool_size": 10,
    "max_pool_size": 30,
    "max_queue_size": 100
  }
}'
),
('__default_uuid__', 'CUSTOM_CONFIG', '{"config": false}');
