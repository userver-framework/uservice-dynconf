import pytest

from testsuite.databases import pgsql

@pytest.mark.parametrize(
    "service,config_value,config_name,new_value", [
        ("main-service", "false", "main-varibale", "true"),
    ],
)
@pytest.mark.pgsql('uservice_dynconf', files=['default_configs.sql'])
async def test_default_values(pgsql, load_json, service, config_value, config_name, new_value):
    cursor = pgsql['uservice_dynconf'].cursor()
    cursor.execute(
        'INSERT INTO uservice_dynconf.configs (service, config_value, config_name) '
        ' VALUES ($1, $2, $3) RETURNING uuid', (service, config_value, config_name)
    )
    uuid = cursor.fetchone()
    response = await service_client.patch(
        '/admin/v1/variables/' + string(uuid),  json={'value': new_value},
    )
    assert response.status_code == 200

    cursor.execute(
        'SELECT config_value FROM uservice_dynconf.configs WHERE uuid=$1', (uuid, )
    )
    value = cursor.fetchone()
    assert value == new_value
