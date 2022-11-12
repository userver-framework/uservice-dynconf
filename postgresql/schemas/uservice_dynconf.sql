DROP SCHEMA IF EXISTS uservice_dynconf CASCADE;
CREATE SCHEMA IF NOT EXISTS uservice_dynconf;

CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

CREATE TABLE IF NOT EXISTS uservice_dynconf.configs (
    uuid TEXT NOT NULL UNIQUE DEFAULT uuid_generate_v4(),
    service TEXT NOT NULL DEFAULT '__default__',
    config_name TEXT NOT NULL,
    config_value JSONB,
    created_at timestamptz DEFAULT NOW(),
    updated_at timestamptz DEFAULT NOW(),

    PRIMARY KEY (service, config_name)
);

CREATE INDEX IF NOT EXISTS idx__created_at__configs
ON uservice_dynconf.configs USING btree (created_at);

CREATE INDEX IF NOT EXISTS idx__updated_at__configs
ON uservice_dynconf.configs USING btree (updated_at);

CREATE UNIQUE INDEX IF NOT EXISTS idx__pair_service_and_connfig
ON uservice_dynconf.configs USING btree (service, config_name);


CREATE TABLE IF NOT EXISTS uservice_dynconf.users_x_configs (
    "user" TEXT NOT NULL,
    service TEXT NOT NULL,
    permission TEXT
);
