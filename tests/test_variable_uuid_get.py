import pytest

from testsuite.databases import pgsql

@pytest.mark.parametrize(
    "service,config_value,config_name", [
        ("main-service", "false", "main-varibale"),
    ],
)
@pytest.mark.pgsql('uservice_dynconf', files=['default_configs.sql'])
async def test_default_values(pgsql, load_json, service, config_value, config_name):
    cursor = pgsql['uservice_dynconf'].cursor()
    cursor.execute(
        'INSERT INTO uservice_dynconf.configs (service, config_value, config_name) '
        ' VALUES ($1, $2, $3) RETURNING uuid', service, config_value, config_name
    )
    uuid = cursor.fetchone()
    response = await service_client.get(
        '/admin/v1/variables/' + string(uuid),
    )
    assert response.status_code == 200
    assert response.json()['name'] == config_name
    assert response.json()['value'] == config_value
    assert response.json()['service'] == config_value

@pytest.mark.parametrize(
    "service,config_value,config_name", [
        ("main-service", "10000", "main-varibale"),
    ],
)
@pytest.mark.pgsql('uservice_dynconf', files=['default_configs.sql'])
async def test_default_values(pgsql, load_json, service, config_value, config_name):
    response = await service_client.get(
        '/admin/v1/variables/' + string(uuid),
    )
    assert response.status_code == 404

@pytest.mark.parametrize(
    "service,config_value,config_name", [
        ("main-service", "main-value", "main-varibale"),
    ],
)
@pytest.mark.pgsql('uservice_dynconf', files=['default_configs.sql'])
async def test_default_values(pgsql, load_json, service, config_value, config_name):
    response = await service_client.get(
        '/admin/v1/variables/' + string(uuid),
    )
    assert response.status_code == 400