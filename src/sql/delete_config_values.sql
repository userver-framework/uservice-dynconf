DELETE FROM uservice_dynconf.configs
WHERE service = $1 
  AND config_name IN (
    SELECT unnest($2)
  );
