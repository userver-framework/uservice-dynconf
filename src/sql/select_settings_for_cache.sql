SELECT 
    (service, config_name), 
    config_value, 
    config_mode, 
    updated_at
FROM 
    uservice_dynconf.configs;
