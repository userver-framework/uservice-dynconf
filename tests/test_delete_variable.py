import pytest
from testsuite.databases import pgsql


async def test_delition_no_uuid(service_client):
    response = await service_client.delete("/api/v1/variables/")
    assert response.status_code == 400


@pytest.mark.pgsql(
    'uservice_dynconf',
    files=['default_configs.sql', 'custom_configs.sql'],
)
async def test_delition(service_client, pgsql):
    cursor = pgsql["uservice_dynconf"].cursor()
    cursor.execute("SELECT uuid FROM uservice_dynconf.configs LIMIT 1;")
    uuid = cursor.fetchone()[0]
    response = await service_client.delete(f"/api/v1/variables/{uuid}")
    assert response.status_code == 200
    cursor.execute(
        "SELECT COUNT(*) FROM uservice_dynconf.configs WHERE uuid=%s;",
        (uuid,))
    assert cursor.fetchone()[0] == 0


@pytest.mark.pgsql(
    'uservice_dynconf',
    files=['default_configs.sql', 'custom_configs.sql'],
)
async def test_delition_double_delete(service_client, pgsql):
    cur = pgsql["uservice_dynconf"].cursor()
    cur.execute("SELECT uuid FROM uservice_dynconf.configs LIMIT 1;")
    record = cur.fetchone()[0]
    response = await service_client.delete(f"/api/v1/variables/{record}")
    assert response.status_code == 200
    response = await service_client.delete(f"/api/v1/variables/{record}")
    assert response.status_code == 404
    cur.execute(
        "SELECT COUNT(*) FROM uservice_dynconf.configs WHERE uuid=%s;",
        (record,))
    assert cur.fetchone()[0] == 0
