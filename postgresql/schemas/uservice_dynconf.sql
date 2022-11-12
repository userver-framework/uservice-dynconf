DROP SCHEMA IF EXISTS uservice_dynconf CASCADE;
CREATE SCHEMA IF NOT EXISTS uservice_dynconf;

CREATE TABLE IF NOT EXISTS uservice_dynconf.configs (
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

-- new 
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

CREATE TABLE IF NOT EXISTS uservice_dynconf.variables (
    "uuid" TEXT PRIMARY KEY DEFAULT uuid_generate_v4(),
    "config" TEXT,
    "name" TEXT NOT NULL,
    "value" JSONB,
    "created_at" timestamptz DEFAULT NOW(),
    "updated_at" timestamptz DEFAULT NOW(),
    CONSTRAINT fk_variables__config
		FOREIGN KEY("config")
			REFERENCES uservice_dynconf.configs_(uuid)
);

CREATE INDEX IF NOT EXISTS idx__created_at__configs
ON uservice_dynconf.variables USING btree ("created_at");

CREATE INDEX IF NOT EXISTS idx__updated_at__configs
ON uservice_dynconf.variables USING btree ("updated_at");

CREATE TABLE IF NOT EXISTS uservice_dynconf.configs_ (
    "uuid" TEXT PRIMARY KEY DEFAULT uuid_generate_v4(),
    "name" TEXT
);

CREATE TABLE IF NOT EXISTS uservice_dynconf.users_x_configs (
    "user" TEXT NOT NULL,
    "config" TEXT NOT NULL,
    CONSTRAINT fk_users_x_configs__config
		FOREIGN KEY("config")
			REFERENCES uservice_dynconf.configs_("uuid")
    "permission" TEXT
);
