WITH new_service AS (
  INSERT INTO uservice_dynconf.services (uuid, service_name) VALUES ('__custom_uuid__', 'my-custom-service') RETURNING uuid
)
INSERT INTO uservice_dynconf.configs (service_uuid, config_name, config_value)
VALUES 
('__custom_uuid__', 'USERVER_RPS_CCONTROL_ENABLED', 'true'),
('__custom_uuid__', 'POSTGRES_CONNECTION_POOL_SETTINGS', 
'{
  "__default__": {
    "min_pool_size": 10,
    "max_pool_size": 30,
    "max_queue_size": 100
  }
}'
),
('__custom_uuid__', 'CUSTOM_CONFIG', '{"config": false}');
