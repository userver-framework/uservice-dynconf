SELECT 
    service,
    config_name,
    config_value::text
FROM 
    uservice_dynconf.configs
WHERE
    config_mode = 'dynamic_config'::uservice_dynconf.mode
