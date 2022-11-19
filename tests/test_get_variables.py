import pytest

url = 'admin/v2/variables'

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
    response = await service_client.get(url, headers=header)
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
    response = await service_client.get(url, headers=header1)
    assert response.status == 200
    assert len(response.json().get('items')) == 0
    assert response.json().get('total') == 24
    header1 = {'page': '1'}
    response = await service_client.get(url, headers=header1)
    assert response.status == 200
    assert len(response.json().get('items')) == 24
    assert response.json().get('total') == 24
    header2 = {'limit': '15'}
    response = await service_client.get(url, headers=header2)
    assert response.status == 200
    assert len(response.json().get('items')) == 15
    assert response.json().get('total') == 24
    header3 = {'page': '2', 'limit': '2'}
    response = await service_client.get(url, headers=header3)
    assert response.status == 200
    assert len(response.json().get('items')) == 2
    assert response.json().get('total') == 24
    #assert response.json().get('items') == [{"uuid":"aad06c83-8fb9-4afe-97b2-06fd03302857","service":"__default__","config_name":"USERVER_TASK_PROCESSOR_PROFILER_DEBUG","updated_at":"2022-11-13T09:13:49.840703+00:00"},
    #                                        {"uuid":"e4f74c57-d555-481e-b908-a5098b0bead4","service":"__default__","config_name":"USERVER_TASK_PROCESSOR_QOS","updated_at":"2022-11-13T09:13:49.840703+00:00"}]