SELECT
service, config_name, config_value::text,
    (config_mode != 'dynamic_config' ::uservice_dynconf.mode
    ) as is_kill_switch FROM uservice_dynconf.configs ORDER BY config_name,
    service
