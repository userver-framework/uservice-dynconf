import pytest
from uuid import uuid4
from testsuite.databases import pgsql


@pytest.mark.parametrize(
    "service,config_value,config_name", [
        ("main-service", "false", "main-varibale"),
    ],
)
@pytest.mark.pgsql('uservice_dynconf', files=['default_configs.sql'])
async def test_default_values(service_client, pgsql, load_json, service,
                              config_value, config_name):
    cursor = pgsql['uservice_dynconf'].cursor()
    cursor.execute(
        'INSERT INTO uservice_dynconf.services (service_name, config_value, '
        ' VALUES (%s) RETURNING uuid', (service)
    )
    service_uuid = cursor.fetchone()
    cursor.execute(
        'INSERT INTO uservice_dynconf.configs (service_uuid, config_value, '
        'config_name) '
        ' VALUES (%s, %s, %s) RETURNING uuid', (service_uuid,
                                                config_value, config_name)
    )
    uuid = cursor.fetchone()
    response = await service_client.get(
        '/admin/v2/configs/' + str(uuid),
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
async def test_default_values(service_client, pgsql, load_json, service,
                              config_value, config_name):
    uuid = str(uuid4())
    response = await service_client.get(
        '/admin/v2/configs/' + str(uuid),
    )
    assert response.status_code == 404
