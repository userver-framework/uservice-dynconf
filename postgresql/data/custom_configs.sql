-- See https://userver.tech/dd/d2c/md_en_schemas_dynamic_configs.html
-- for description of dynamic configs.

INSERT INTO uservice_dynconf.configs (config_name, config_value, uuid)
VALUES ('HTTP_CLIENT_EXCUDE_EXTRA_HEADER', 'true', '3e897b1d-604d-4464-ac6c-a672ac17b137')
ON CONFLICT (service, config_name)
DO NOTHING;
