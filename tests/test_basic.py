# Start via `make test-debug` or `make test-release`
async def test_basic(service_dynamic_configs_client):
    response = await service_dynamic_configs_client.post('/hello', json={})
    assert response.status == 200
