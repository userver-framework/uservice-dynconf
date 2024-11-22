INSERT INTO uservice_dynconf.configs
    (service, config_name, config_value, config_mode)
SELECT 
    $1, 
    d.key, 
    d.value::jsonb, 
    m.value::uservice_dynconf.mode
FROM 
    jsonb_each_text($2) AS d,
    jsonb_each_text($3) AS m
WHERE 
    d.key = m.key
ON CONFLICT (service, config_name)
DO UPDATE SET
    config_value = EXCLUDED.config_value,
    config_mode = EXCLUDED.config_mode,
    updated_at = NOW();
