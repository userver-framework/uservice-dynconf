DROP SCHEMA IF EXISTS service_dynamic_configs CASCADE;

CREATE SCHEMA IF NOT EXISTS service_dynamic_configs;


CREATE TABLE IF NOT EXISTS service_dynamic_configs.configs (
    service TEXT NOT NULL DEFAULT '__default__',
    config_name TEXT NOT NULL,
    config_value JSONB,
    created_at timestamptz DEFAULT NOW(),
    updated_at timestamptz DEFAULT NOW(),

    PRIMARY KEY (service, config_name)
);

CREATE INDEX IF NOT EXISTS idx__created_at__configs
ON service_dynamic_configs.configs USING btree (created_at);

CREATE INDEX IF NOT EXISTS idx__updated_at__configs
ON service_dynamic_configs.configs USING btree (updated_at);

CREATE UNIQUE INDEX IF NOT EXISTS idx__pair_service_and_connfig
ON service_dynamic_configs.configs USING btree (service, config_name);
