DROP SCHEMA IF EXISTS uservice_dynconf CASCADE;
CREATE SCHEMA IF NOT EXISTS uservice_dynconf;
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
			REFERENCES uservice_dynconf.configs(uuid)
);

CREATE INDEX IF NOT EXISTS idx__created_at__configs
ON uservice_dynconf.variables USING btree ("created_at");

CREATE INDEX IF NOT EXISTS idx__updated_at__configs
ON uservice_dynconf.variables USING btree ("updated_at");

CREATE TABLE IF NOT EXISTS uservice_dynconf.configs (
    "uuid" TEXT PRIMARY KEY DEFAULT uuid_generate_v4(),
    "name" TEXT
);

CREATE TABLE IF NOT EXISTS uservice_dynconf.users_x_configs (
    "user" TEXT NOT NULL,
    "config" TEXT NOT NULL,
    CONSTRAINT fk_users_x_configs__user
		FOREIGN KEY("user")
			REFERENCES uservice_dynconf.users("uuid"),
    CONSTRAINT fk_users_x_configs__config
		FOREIGN KEY("config")
			REFERENCES uservice_dynconf.configs("uuid")
    "permission" TEXT
);

CREATE TABLE IF NOT EXISTS uservice_dynconf.users (
    "uuid" TEXT PRIMARY KEY DEFAULT uuid_generate_v4(),
    "login" TEXT NOT NULL UNIQUE,
    "password_hash" TEXT NOT NULL
);
