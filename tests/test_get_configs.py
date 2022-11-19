import pytest

url = '/admin/v1/configs'


@pytest.mark.parametrize('query_params',
                         [
                             ({'limit': '-3'}),
                             ({'offset': '-7'}),
                             ({'limit': '-3', 'offset': '-7'}),
                             ({'limit': 'kek'}),
                             ({'offset': 'kek'}),
                             ({'limit': 'kek', 'offset': 'kek'})
                         ]
                         )
@pytest.mark.pgsql('uservice_dynconf', files=['default_configs.sql'])
async def test_bad_head(service_client, query_params):
    response = await service_client.get(url, params=query_params)
    assert response.status == 400


@pytest.mark.pgsql('uservice_dynconf', files=['default_configs.sql'])
async def test_get_ok(service_client):
    response = await service_client.get(url)
    assert response.status == 200
    assert response.json().get('count') == 1
    assert response.json().get('total') == 1
    assert response.json().get('items') == ["__default__"]


@pytest.mark.pgsql('uservice_dynconf', files=['default_configs.sql'])
async def test_get_ok_with_params(service_client):
    query_params1 = {'offset': '1'}
    response = await service_client.get(url, params=query_params1)
    assert response.status == 200
    assert response.json().get('count') == 0
    assert response.json().get('total') == 1
    query_params2 = {'limit': '0'}
    response = await service_client.get(url, params=query_params2)
    assert response.status == 200
    assert response.json().get('count') == 0
    assert response.json().get('total') == 1
    query_params3 = {'offset': '0', 'limit': '1'}
    response = await service_client.get(url, params=query_params3)
    assert response.status == 200
    assert response.json().get('count') == 1
    assert response.json().get('total') == 1
    assert response.json().get('items') == ["__default__"]