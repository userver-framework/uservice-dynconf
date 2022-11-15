import pytest

url = '/admin/v1/variables'


@pytest.mark.parametrize('query_params',
                         [
                             ({'limit': '-3'}),
                             ({'offset': '-7'}),
                             ({'limit': '-3', 'OFFSET': '-7'}),
                             ({'limit': 'kek'}),
                             ({'offset': 'kek'}),
                             ({'limit': 'kek', 'OFFSET': 'kek'})
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
    assert response.json().get('count') == 24
    assert response.json().get('total') == 24


@pytest.mark.pgsql('uservice_dynconf', files=['default_configs.sql'])
async def test_get_ok_with_params(service_client):
    query_params1 = {'offset': '19'}
    response = await service_client.get(url, params=query_params1)
    assert response.status == 200
    assert response.json().get('count') == 5
    assert response.json().get('total') == 24
    query_params2 = {'limit': '15'}
    response = await service_client.get(url, params=query_params2)
    assert response.status == 200
    assert response.json().get('count') == 15
    assert response.json().get('total') == 24
    query_params3 = {'offset': '22', 'limit': '2'}
    response = await service_client.get(url, params=query_params3)
    assert response.status == 200
    assert response.json().get('count') == 2
    assert response.json().get('total') == 24
