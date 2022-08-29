import pytest

from testsuite.databases import pgsql

@pytest.mark.pgsql('uservice_dynconf', files=['default_configs.sql'])
async def test_default_values(pgsql, load_json):
    cursor = pgsql['uservice_dynconf_default'].cursor()
    cursor.execute(
        'SELECT json_object(config_name, config_value) FROM uservice_dynconf.configs',
    )
    data = cursor.fetchall()
    assert len(data) == 1
    assert len(data[0]) == 1
    json_data = data[0][0]
    
    defaults = load_json('configs/dynamic_config_fallback.json')
    
    assert json_data == defaults
