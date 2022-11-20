import pytest
from testsuite.databases import pgsql

@pytest.mark.pgsql(
    'uservice_dynconf',
    files=['default_configs.sql'],
)
async def test_config_values(service_client, pgsql):
    response = await service_client.post(
        '/configs/values',
    )

    assert response.status_code == 200
