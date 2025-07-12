import pytest

from testsuite.databases import pgsql


# Make sure that the defaults in default_configs.sql match values in
# dynamic_config_fallback.json and there's no additional values in the
# latter.
@pytest.mark.pgsql('uservice_dynconf', files=['default_configs.sql'])
async def test_default_values(pgsql, load_json):
    cursor = pgsql['uservice_dynconf'].cursor()
    cursor.execute(
        'SELECT json_object_agg(config_name, config_value) '
        'FROM uservice_dynconf.configs WHERE service=\'__default__\'',
    )
    data = cursor.fetchall()
    assert len(data) == 1
    assert len(data[0]) == 1
    db_defaults = data[0][0]

    service_defaults = load_json('dynamic_config_fallback.json')

    for key, value in service_defaults.items():
        assert key in db_defaults
        assert db_defaults[key] == value, 'At key: ' + key


# Make sure that the defaults can be retrieved from admin handler.
@pytest.mark.pgsql('uservice_dynconf', files=['default_configs.sql'])
async def test_default_values_smoke(pgsql, service_client):
    response = await service_client.post('/admin/v1/configs/get', json={})
    response_json = response.json();
    assert len(response_json) > 1
    assert response_json[0]['service'] == '__default__'

    cursor = pgsql['uservice_dynconf'].cursor()
    cursor.execute('SELECT COUNT(*) FROM uservice_dynconf.configs')
    data = cursor.fetchall()
    assert len(response.json()) == data[0][0]
