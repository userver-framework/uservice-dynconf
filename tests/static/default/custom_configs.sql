WITH new_service AS (
  INSERT INTO uservice_dynconf.services (service_name) VALUES ('my-custom-service') RETURNING uuid
)
INSERT INTO uservice_dynconf.configs (service_uuid, config_name, config_value)
VALUES 
((SELECT uuid FROM new_service), 'USERVER_RPS_CCONTROL_ENABLED', 'true'),
((SELECT uuid FROM new_service), 'POSTGRES_CONNECTION_POOL_SETTINGS', 
'{
  "__default__": {
    "min_pool_size": 10,
    "max_pool_size": 30,
    "max_queue_size": 100
  }
}'
),
((SELECT uuid FROM new_service), 'CUSTOM_CONFIG', '{"config": false}');
