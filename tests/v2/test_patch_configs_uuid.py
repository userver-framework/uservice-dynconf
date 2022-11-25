import pytest

from testsuite.databases import pgsql


@pytest.mark.parametrize(
    "service,config_value,config_name,new_value", [
        ("main-service", "false", "main-varibale", "true"),
    ],
)
@pytest.mark.pgsql('uservice_dynconf', files=['default_configs.sql'])
async def test_default_values(service_client, pgsql, load_json, service,
                              config_value, config_name, new_value):
    cursor = pgsql['uservice_dynconf'].cursor()
    cursor.execute(
        'INSERT INTO uservice_dynconf.services (service_name) '
        ' VALUES (%s) RETURNING uuid', (service,)
    )
    service_uuid = cursor.fetchone()
    cursor.execute(
        'INSERT INTO uservice_dynconf.configs (service_uuid, config_value, '
        'config_name) '
        ' VALUES (%s, %s, %s) RETURNING uuid', (service_uuid,
                                                config_value, config_name)
    )
    uuid = cursor.fetchone()[0]
    response = await service_client.patch(
        '/admin/v2/configs/' + str(uuid),  json={'value': new_value},
    )
    assert response.status_code == 200

    cursor.execute(
        'SELECT config_value FROM uservice_dynconf.configs WHERE uuid=%s', (
            uuid, )
    )
    value = cursor.fetchone()
    assert value[0] is True  # new_value value