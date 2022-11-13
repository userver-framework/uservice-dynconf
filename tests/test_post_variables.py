import pytest
from testsuite.databases import pgsql


class PostgresqlClient:
    def __init__(self, connection):
        self.connection = connection

    def variable_is_present(self, uuid, service_name, config_name):
        cursor = self.connection.cursor()
        cursor.execute('SELECT service, config_name FROM '
                       'uservice_dynconf.configs WHERE uuid = %s', (uuid, ))
        service_res, config_res = cursor.fetchone()
        if service_res == service_name and config_res == config_name:
            return True
        return False


@pytest.fixture(scope='function')
def db_setup(pgsql):
    conn = pgsql['uservice_dynconf'].conn
    return PostgresqlClient(conn)


@pytest.mark.parametrize(
    'input, conf_name',
    [
        pytest.param({
            "name": "CUSTOM_CONFIG",
            "service": "my-service",
            "value": '{"property1": 10,'
                       '"property2": '
                       '{"info": "info"}}'
        }, "CUSTOM_CONFIG", id='add json'),
        pytest.param({
            "name": "CUSTOM_CONFIG_2",
            "service": "my-service",
            "value": "true"
        }, "CUSTOM_CONFIG_2", id='add bool'),
        pytest.param({
            "name": "CUSTOM_CONFIG_3",
            "service": "my-service",
            "value": "1000"
        }, "CUSTOM_CONFIG_3", id='add int'),
    ])
async def test_post_variable_positive(service_client,
                                      db_setup, input, conf_name):
    """Add new config variable"""
    response = await service_client.post(
        '/admin/v1/variables', json=input
    )
    assert response.status_code == 200
    c_uuid = response.json()["uuid"]
    assert db_setup.variable_is_present(c_uuid, 'my-service', conf_name)
