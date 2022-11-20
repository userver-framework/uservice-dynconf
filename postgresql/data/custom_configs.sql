-- See https://userver.tech/dd/d2c/md_en_schemas_dynamic_configs.html
-- for description of dynamic configs.

INSERT INTO uservice_dynconf.configs (config_name, config_value, service_uuid)
VALUES ('HTTP_CLIENT_EXCUDE_EXTRA_HEADER', 'true', (SELECT uuid FROM new_service))
ON CONFLICT ON CONSTRAINT unique_in_configs
DO NOTHING;
