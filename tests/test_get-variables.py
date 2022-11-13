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
    # assert response.json().get('items') == [{"uuid":"aad06c83-8fb9-4afe-97b2-06fd03302857","service":"__default__","config_name":"USERVER_TASK_PROCESSOR_PROFILER_DEBUG","updated_at":"2022-11-13T09:13:49.840703+00:00"},
    #                                         {"uuid":"e4f74c57-d555-481e-b908-a5098b0bead4","service":"__default__","config_name":"USERVER_TASK_PROCESSOR_QOS","updated_at":"2022-11-13T09:13:49.840703+00:00"}]