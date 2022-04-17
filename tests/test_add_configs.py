import pytest

from testsuite.databases import pgsql
from testsuite import utils


@pytest.mark.parametrize(
    'ids, configs',
    [
        pytest.param(
            ['CUSTOM_CONFIG'],
            {'CUSTOM_CONFIG': {'config': 'value'}},
            id='add one config',
        ),
        pytest.param(
            ['CUSTOM_CONFIG', 'ADD_CONFIG', 'MORE_CONFIGS'],
            {
                'CUSTOM_CONFIG': {'config': 'value'},
                'ADD_CONFIG': 5000,
                'MORE_CONFIGS': {
                    'all': {},
                    'we': {'state': False},
                    'data': 'nor,',
                },
            },
            id='add bulk configs',
        ),
    ],
)
async def test_configs_add_values(
        service_dynamic_configs_client, invalidate_caches, ids, configs,
):
    service = 'my-service'
    await invalidate_caches()
    response = await service_dynamic_configs_client.post(
        '/configs/values', json={'ids': ids, 'service': service},
    )
    assert response.status_code == 200
    assert response.json()['configs'] == {}

    response = await service_dynamic_configs_client.post(
        '/admin/v1/configs', json={'service': service, 'configs': configs},
    )

    response.status_code == 204

    await invalidate_caches()
    response = await service_dynamic_configs_client.post(
        '/configs/values', json={'ids': ids, 'service': service},
    )
    assert response.status_code == 200
    assert response.json()['configs'] == configs


@pytest.mark.pgsql(
    'service_dynamic_configs',
    files=['default_configs.sql', 'custom_configs.sql'],
)
async def test_redefinitions_configs(
        service_dynamic_configs_client, invalidate_caches,
):
    await invalidate_caches()
    ids = ['CUSTOM_CONFIG', 'MORE_CONFIGS']
    service = 'my-custom-service'
    response = await service_dynamic_configs_client.post(
        '/configs/values', json={'ids': ids, 'service': service},
    )
    assert response.status_code == 200
    assert response.json()['configs'] == {'CUSTOM_CONFIG': {'config': False}}

    configs = {
        'CUSTOM_CONFIG': {'config': True, 'data': {}, 'status': 99},
        'MORE_CONFIGS': {'__state__': 'norm', 'enabled': True, 'data': 22.22},
    }
    response = await service_dynamic_configs_client.post(
        '/admin/v1/configs', json={'service': service, 'configs': configs},
    )

    response.status_code == 204

    await invalidate_caches()
    response = await service_dynamic_configs_client.post(
        '/configs/values', json={'ids': ids, 'service': service},
    )
    assert response.status_code == 200
    assert response.json()['configs'] == configs


@pytest.mark.parametrize(
    'request_data',
    [
        ({}),
        ({'configs': {}}),
        ({'configs': {'CONFIG': 1000}}),
        ({'service': ''}),
        ({'service': 'my-service'}),
        ({'configs': {'CONFIG': 1000}, 'service': ''}),
        ({'configs': {}, 'service': 'my-service'}),
    ],
)
async def test_add_configs_400(
        service_dynamic_configs_client, request_data,
):
    response = await service_dynamic_configs_client.post(
        '/admin/v1/configs', json=request_data,
    )
    assert response.status_code == 400
    assert response.json() == {
        'code': '400',
        'message': 'Fields \'configs\' and \'service\' are required',
    }
