import pytest

from testsuite.databases import pgsql


@pytest.mark.parametrize(
    "service1, service2, config_name, config_value", [
        ("my_service", "amogus", "my_config", "true"),
    ]
)
async def test_correct_clone(service_client, pgsql, service1, service2,
                             config_name, config_value):
    cursor = pgsql['uservice_dynconf'].cursor()
    cursor.execute(
        'INSERT INTO uservice_dynconf.services '
        '(service_name) '
        'VALUES (%s) RETURNING uuid', (service1,)
    )
    service_uuid = cursor.fetchone()[0]
    cursor.execute(
        'INSERT INTO uservice_dynconf.services '
        '(service_name) '
        'VALUES (%s) RETURNING uuid', (service2,)
    )
    cursor.execute(
        'INSERT INTO uservice_dynconf.configs '
        '(service_uuid,config_name,config_value) '
        'VALUES (%s, %s, %s) RETURNING uuid',
        (service_uuid, config_name, config_value)
    )
    uuid = cursor.fetchone()[0]

    response = await service_client.post(
        '/admin/v2/configs/' + str(uuid) + '/clone',
        json={'service_name': 'amogus'},
    )

    assert response.status_code == 200
    assert 'config_uuid' in response.json()


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
        '/admin/v2/configs/' + uuid + '/clone',  json={'service_name': service},
    )

    assert response.status_code == 400


@pytest.mark.parametrize(
    "service, config_name, config_value", [
        ("my_service", "my_config", "true"),
    ]
)
@pytest.mark.pgsql('uservice_dynconf', files=['default_configs.sql'])
async def test_config_not_exists(service_client, pgsql, service,
                                 config_name, config_value):
    cursor = pgsql['uservice_dynconf'].cursor()
    cursor.execute(
        'INSERT INTO uservice_dynconf.services '
        '(service_name) '
        'VALUES (%s) RETURNING uuid', (service,)
    )
    service_uuid = cursor.fetchone()[0]
    cursor.execute(
        'INSERT INTO uservice_dynconf.configs '
        '(service_uuid,config_name,config_value) '
        'VALUES (%s, %s, %s) RETURNING uuid',
        (service_uuid, config_name, config_value)
    )
    response = await service_client.post(
        '/admin/v2/configs/bad_uuid/clone',  json={'service_name': service},
    )

    assert response.status_code == 404


@pytest.mark.parametrize(
    "service1, service2, config_name, config_value", [
        ("my_service", "amogus", "my_config", "true"),
    ]
)
async def test_service_has_config(service_client, pgsql,
                                  service1, service2,
                                  config_name, config_value):
    cursor = pgsql['uservice_dynconf'].cursor()
    cursor.execute(
        'INSERT INTO uservice_dynconf.services '
        '(service_name) '
        'VALUES (%s) RETURNING uuid', (service1,)
    )
    service_uuid1 = cursor.fetchone()[0]
    cursor.execute(
        'INSERT INTO uservice_dynconf.services '
        '(service_name) '
        'VALUES (%s) RETURNING uuid', (service2,)
    )
    service_uuid2 = cursor.fetchone()[0]
    cursor.execute(
        'INSERT INTO uservice_dynconf.configs '
        '(service_uuid,config_name,config_value) '
        'VALUES (%s, %s, %s) RETURNING uuid',
        (service_uuid1, config_name, config_value)
    )
    uuid = cursor.fetchone()[0]
    cursor.execute(
        'INSERT INTO uservice_dynconf.configs '
        '(service_uuid,config_name,config_value) '
        'VALUES (%s, %s, %s) RETURNING uuid',
        (service_uuid2, config_name, config_value)
    )

    response = await service_client.post(
        '/admin/v2/configs/' + str(uuid) + '/clone',
        json={'service_name': 'amogus'},
    )

    assert response.status_code == 409


@pytest.mark.parametrize(
    "service1, service2, config_name, config_value", [
        ("my_service", "amogus", "my_config", "true"),
    ]
)
async def test_service_not_exist_ok(service_client, pgsql,
                                    service1, service2,
                                    config_name, config_value):
    cursor = pgsql['uservice_dynconf'].cursor()
    cursor.execute(
        'INSERT INTO uservice_dynconf.services '
        '(service_name) '
        'VALUES (%s) RETURNING uuid', (service1,)
    )
    service_uuid = cursor.fetchone()[0]
    cursor.execute(
        'INSERT INTO uservice_dynconf.configs '
        '(service_uuid,config_name,config_value) '
        'VALUES (%s, %s, %s) RETURNING uuid',
        (service_uuid, config_name, config_value)
    )
    uuid = cursor.fetchone()[0]

    response = await service_client.post(
        '/admin/v2/configs/' + str(uuid) + '/clone',
        json={'service_name': 'amogus'},
    )

    assert response.status_code == 200
    assert 'config_uuid' in response.json()
    assert 'service_uuid' in response.json()


@pytest.mark.parametrize(
    "service1, service2, config_name", [
        ("my_service", "amogus", "my_config"),
    ]
)
async def test_clone_empty_config_value(service_client,
                                        pgsql, service1, service2,
                                        config_name):
    cursor = pgsql['uservice_dynconf'].cursor()
    cursor.execute(
        'INSERT INTO uservice_dynconf.services '
        '(service_name) '
        'VALUES (%s) RETURNING uuid', (service1,)
    )
    service_uuid = cursor.fetchone()[0]
    cursor.execute(
        'INSERT INTO uservice_dynconf.services '
        '(service_name) '
        'VALUES (%s) RETURNING uuid', (service2,)
    )
    cursor.execute(
        'INSERT INTO uservice_dynconf.configs '
        '(service_uuid,config_name) '
        'VALUES (%s, %s) RETURNING uuid',
        (service_uuid, config_name)
    )
    uuid = cursor.fetchone()[0]

    response = await service_client.post(
        '/admin/v2/configs/' + str(uuid) + '/clone',
        json={'service_name': 'amogus'},
    )

    assert response.status_code == 200
    assert 'config_uuid' in response.json()
    assert 'service_uuid' in response.json()

