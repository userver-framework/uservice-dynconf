import pytest

url = '/admin/v1/variables'


@pytest.mark.parametrize('header',
                         [
                             ({'LIMIT': '-3'}),
                             ({'OFFSET': '-7'}),
                             ({'LIMIT': '-3', 'OFFSET': '-7'}),
                             ({'LIMIT': 'kek'}),
                             ({'OFFSET': 'kek'}),
                             ({'LIMIT': 'kek', 'OFFSET': 'kek'})
                         ]
                         )
@pytest.mark.pgsql('uservice_dynconf', files=['default_configs.sql'])
async def test_bad_head(service_client, header):
    response = await service_client.get(url, headers=header)
    assert response.status == 400


@pytest.mark.pgsql('uservice_dynconf', files=['default_configs.sql'])
async def test_get_ok(service_client):
    response = await service_client.get(url)
    assert response.status == 200
    assert response.json().get('count') == 24
    assert response.json().get('total') == 24


@pytest.mark.pgsql('uservice_dynconf', files=['default_configs.sql'])
async def test_get_ok_with_params(service_client):
    header1 = {'OFFSET': '19'}
    response = await service_client.get(url, headers=header1)
    assert response.status == 200
    assert response.json().get('count') == 5
    assert response.json().get('total') == 24
    header2 = {'LIMIT': '15'}
    response = await service_client.get(url, headers=header2)
    assert response.status == 200
    assert response.json().get('count') == 15
    assert response.json().get('total') == 24
    header3 = {'OFFSET': '22', 'LIMIT': '2'}
    response = await service_client.get(url, headers=header3)
    assert response.status == 200
    assert response.json().get('count') == 2
    assert response.json().get('total') == 24
