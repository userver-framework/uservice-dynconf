#pragma once

namespace uservice_dynconf::sql {

// selects for cache
inline constexpr std::string_view kSelectConfigForCache = R"~(
SELECT (uuid), service_uuid, config_name, config_value, updated_at
FROM uservice_dynconf.configs
)~";

inline constexpr std::string_view kSelectServiceForCache = R"~(
SELECT (uuid), service_name, updated_at
FROM uservice_dynconf.services
)~";

// v1
inline constexpr std::string_view kInsertConfigValue = R"~(
WITH uuid_service AS (
    INSERT INTO uservice_dynconf.services (service_name) 
    VALUES ($1) RETURNING uuid
)
INSERT INTO uservice_dynconf.configs (service_uuid, config_name, config_value) 
SELECT (SELECT uuid FROM new_service)::TEXT, d.key, d.value::jsonb
FROM jsonb_each_text($2) as d
ON CONFLICT ON CONSTRAINT unique_in_configs
DO UPDATE SET config_value = EXCLUDED.config_value, updated_at = NOW()
)~";

inline constexpr std::string_view kDeleteConfigValues = R"~(
WITH uuid_service AS (
    SELECT uuid FROM uservice_dynconf.services WHERE service_name=$1
)
DELETE FROM uservice_dynconf.configs
WHERE service_uuid=(SELECT uuid FROM uuid_service)::TEXT and config_name IN (SELECT UNNEST($2))
)~";

// v2
inline constexpr std::string_view kDeleteConfig = R"~(
DELETE FROM uservice_dynconf.configs
WHERE uuid=$1
)~";

inline constexpr std::string_view kInsertConfig = R"~(
WITH insert_service AS (
    INSERT INTO uservice_dynconf.services (service_name) VALUES ($1)
    ON CONFLICT DO NOTHING
    RETURNING uuid
), uuid_service AS (
    SELECT uuid FROM uservice_dynconf.services WHERE service_name=$1
    UNION ALL
    SELECT uuid FROM insert_service
)
INSERT INTO uservice_dynconf.configs
(service_uuid, config_name, config_value)
VALUES ((SELECT uuid FROM uuid_service)::TEXT, $2, $3::jsonb)
ON CONFLICT ON CONSTRAINT unique_in_configs
DO NOTHING
RETURNING uservice_dynconf.configs.uuid
)~";

inline constexpr std::string_view kSelectAll = R"~(
SELECT uuid, (
    SELECT service_name FROM uservice_dynconf.services 
    WHERE uservice_dynconf.services.uuid=service_uuid
), config_name, updated_at
FROM uservice_dynconf.configs
)~";

inline constexpr std::string_view kSelectServices = R"~(
SELECT service_name 
FROM uservice_dynconf.services
)~";

inline constexpr std::string_view kSelectConfigWithValue = R"~(
SELECT uuid, (
    SELECT service_name FROM uservice_dynconf.services 
    WHERE uservice_dynconf.services.uuid=service_uuid
), config_name, config_value::TEXT 
FROM uservice_dynconf.configs 
WHERE uuid=$1
)~";

inline constexpr std::string_view kUpdateConfig = R"~(
UPDATE uservice_dynconf.configs SET config_value=$2::JSONB, updated_at = now()
WHERE uuid=$1 
RETURNING uuid
)~";
} // namespace uservice_dynconf::sql