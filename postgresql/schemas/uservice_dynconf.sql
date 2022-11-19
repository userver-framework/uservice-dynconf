DROP SCHEMA IF EXISTS uservice_dynconf CASCADE;
CREATE SCHEMA IF NOT EXISTS uservice_dynconf;
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

CREATE TABLE IF NOT EXISTS uservice_dynconf.services (
    uuid TEXT PRIMARY KEY DEFAULT uuid_generate_v4(),
    service_name TEXT NOT NULL UNIQUE,
    created_at timestamptz DEFAULT NOW(),
    updated_at timestamptz DEFAULT NOW()
);

INSERT INTO uservice_dynconf.services (uuid, service_name) VALUES ('__default_uuid__', '__default__');

CREATE TABLE IF NOT EXISTS uservice_dynconf.configs (
    uuid TEXT PRIMARY KEY DEFAULT uuid_generate_v4(),
    service_uuid TEXT NOT NULL DEFAULT '__default_uuid__',
    config_name TEXT NOT NULL,
    config_value JSONB,

    created_at timestamptz DEFAULT NOW(),
    updated_at timestamptz DEFAULT NOW(),

    CONSTRAINT fk_service_in_configs
        FOREIGN KEY (service_uuid)
            REFERENCES uservice_dynconf.services(uuid),
    CONSTRAINT unique_in_configs
        UNIQUE (service_uuid, config_name)
);

CREATE TABLE IF NOT EXISTS uservice_dynconf.users_x_configs (
    "user" TEXT NOT NULL,
    service TEXT NOT NULL,

    CONSTRAINT fk_service_in_users_x_configs
        FOREIGN KEY (service)
            REFERENCES uservice_dynconf.services(uuid)
);