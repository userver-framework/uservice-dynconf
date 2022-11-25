import pytest

url = '/admin/v2/services'


@pytest.mark.parametrize('query_params',
                         [
                             ({'limit': '-3'}),
                             ({'page': '-7'}),
                             ({'limit': '-3', 'page': '-7'}),
                             ({'limit': 'kek'}),
                             ({'page': 'kek'}),
                             ({'limit': 'kek', 'page': 'kek'})
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
    assert len(response.json().get('items')) == 1
    assert response.json().get('total') == 1
    assert response.json().get('items') == [{'service_name': '__default__'}]


@pytest.mark.pgsql('uservice_dynconf', files=['default_configs.sql'])
async def test_get_ok_with_params(service_client):
    query_params1 = {'page': '1'}
    response = await service_client.get(url, params=query_params1)
    assert response.status == 200
    assert len(response.json().get('items')) == 1
    assert response.json().get('total') == 1
    query_params2 = {'limit': '1'}
    response = await service_client.get(url, params=query_params2)
    assert response.status == 200
    assert len(response.json().get('items')) == 1
    assert response.json().get('total') == 1
    query_params3 = {'page': '1', 'limit': '1'}
    response = await service_client.get(url, params=query_params3)
    assert response.status == 200
    assert len(response.json().get('items')) == 1
    assert response.json().get('total') == 1
    assert response.json().get('items') == [{'service_name': '__default__'}]
