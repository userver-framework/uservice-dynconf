INSERT INTO uservice_dynconf.configs (service, config_name, config_value)
VALUES 
('my-custom-service', 'USERVER_RPS_CCONTROL_ENABLED', 'true'),
('my-custom-service', 'POSTGRES_CONNECTION_POOL_SETTINGS', 
'{
  "__default__": {
    "min_pool_size": 10,
    "max_pool_size": 30,
    "max_queue_size": 100
  }
}'
),
('my-custom-service', 'CUSTOM_CONFIG', '{"config": false}');
