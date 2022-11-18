import pytest

from testsuite.databases import pgsql


@pytest.mark.parametrize(
    "service, config_name, config_value", [
        ("my_service", "my_config", "true"),
    ]
)
async def test_correct_clone(service_client, pgsql, service,
                             config_name, config_value):
    cursor = pgsql['uservice_dynconf'].cursor()
    cursor.execute(
        'INSERT INTO uservice_dynconf.configs '
        '(service,config_name,config_value) '
        'VALUES (%s, %s, %s) RETURNING uuid', (service,
                                               config_name, config_value)
    )
    uuid = cursor.fetchone()[0]

    response = await service_client.post(
        '/admin/v1/configs/' + str(uuid) + '/clone',
        json={'service': 'amogus'},
    )

    assert response.status_code == 200
    assert 'uuid' in response.json()


@pytest.mark.parametrize(
    "uuid, service", [
        ('', 'myservice'),
        ('myuuid', ''),
        ('', '')
    ],
)
@pytest.mark.pgsql('uservice_dynconf', files=['default_configs.sql'])
async def test_empty_data(service_client, uuid, service):
    response = await service_client.post(
        '/admin/v1/configs/' + uuid + '/clone',  json={'service': service},
    )

    assert response.status_code == 400


@pytest.mark.parametrize(
    "uuid, service", [
        ('uuid_not_exists', 'myservice')
    ],
)
@pytest.mark.pgsql('uservice_dynconf', files=['default_configs.sql'])
async def test_service_not_exists(service_client, uuid, service):
    response = await service_client.post(
        '/admin/v1/configs/' + uuid + '/clone',  json={'service': service},
    )

    assert response.status_code == 404


@pytest.mark.parametrize(
    "service, config_name, config_value", [
        ("my_service", "my_config", "true"),
    ]
)
async def test_service_name_occupied(service_client, pgsql,
                                     service, config_name, config_value):
    cursor = pgsql['uservice_dynconf'].cursor()
    cursor.execute(
        'INSERT INTO uservice_dynconf.configs '
        '(service, config_name,config_value) '
        'VALUES (%s, %s, %s) RETURNING uuid', (service,
                                               config_name, config_value)
    )
    uuid = cursor.fetchone()[0]

    response = await service_client.post(
        '/admin/v1/configs/' + str(uuid) + '/clone',
        json={'service': 'amogus'},
    )

    assert response.status_code == 200

    response = await service_client.post(
        '/admin/v1/configs/' + str(uuid) + '/clone',
        json={'service': 'amogus'},
    )

    assert response.status_code == 409


@pytest.mark.parametrize(
    "service, config_name", [
        ("my_service", "my_config"),
    ]
)
async def test_clone_empty_config_value(service_client,
                                        pgsql, service, config_name):
    cursor = pgsql['uservice_dynconf'].cursor()
    cursor.execute(
        'INSERT INTO uservice_dynconf.configs (service, config_name) '
        'VALUES (%s, %s) RETURNING uuid', (service, config_name)
    )
    uuid = cursor.fetchone()[0]

    response = await service_client.post(
        '/admin/v1/configs/' + str(uuid) + '/clone',
        json={'service': 'amogus'},
    )

    assert response.status_code == 200
    assert 'uuid' in response.json()
