import pytest

url = 'admin/v2/configs'


@pytest.mark.parametrize('header',
                         [
                             ({'limit': '-3'}),
                             ({'page': '-7'}),
                             ({'limit': '-3', 'page': '-7'}),
                             ({'limit': '0', 'page': '0'}),
                             ({'limit': 'kek'}),
                             ({'page': 'kek'}),
                             ({'limit': 'kek', 'page': 'kek'})
                         ]
                         )
@pytest.mark.pgsql('uservice_dynconf', files=['default_configs.sql'])
async def test_bad_head(service_client, header):
    response = await service_client.get(url, params=header)
    assert response.status == 400


@pytest.mark.pgsql('uservice_dynconf', files=['default_configs.sql'])
async def test_get_ok(service_client):
    response = await service_client.get(url)
    assert response.status == 200
    assert len(response.json().get('items')) == 24
    assert response.json().get('total') == 24


@pytest.mark.pgsql('uservice_dynconf', files=['default_configs.sql'])
async def test_get_ok_with_params(service_client):
    header1 = {'page': '2'}
    response = await service_client.get(url, params=header1)
    assert response.status == 200
    assert len(response.json().get('items')) == 0
    assert response.json().get('total') == 24
    header1 = {'page': '1'}
    response = await service_client.get(url, params=header1)
    assert response.status == 200
    assert len(response.json().get('items')) == 24
    assert response.json().get('total') == 24
    header2 = {'limit': '15'}
    response = await service_client.get(url, params=header2)
    assert response.status == 200
    assert len(response.json().get('items')) == 15
    assert response.json().get('total') == 24
    header3 = {'page': '2', 'limit': '2'}
    response = await service_client.get(url, params=header3)
    assert response.status == 200
    assert len(response.json().get('items')) == 2
    assert response.json().get('total') == 24