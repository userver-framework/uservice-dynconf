import pytest
from testsuite.databases import pgsql

@pytest.mark.pgsql(
    'uservice_dynconf',
    files=['default_configs.sql', 'custom_configs.sql'],
)
async def test_delition(service_client, pgsql):
    response = await service_client.post(
        '/config/values',
    )

    assert response.status_code == 200
