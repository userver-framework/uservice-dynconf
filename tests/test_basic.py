# Start via `make test-debug` or `make test-release`
async def test_basic(service_dynamic_configs_client):
    response = await service_dynamic_configs_client.post('/configs/values', json={})
    assert response.status == 200


async def test_basic_2(service_dynamic_configs_client):
    response = await service_dynamic_configs_client.post('/configs/values', json={
        'service': '__default__'})
    assert response.status == 200
#    assert response.json()['configs'] == {}
