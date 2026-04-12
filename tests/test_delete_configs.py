import pytest

from testsuite.databases import pgsql


@pytest.mark.parametrize(
    'ids, service, configs, expected',
    [
        pytest.param(
            ['CUSTOM_CONFIG'],
            'my-custom-service',
            {'CUSTOM_CONFIG': {'config': False}},
            {},
            id='delete one config',
        ),
        pytest.param(
            ['CUSTOM_CONFIG', 'ADD_CONFIG', 'MORE_CONFIGS'],
            'my-custom-service',
            {'CUSTOM_CONFIG': {'config': False}},
            {},
            id='remove bulk configs',
        ),
        pytest.param(
            [
                'CUSTOM_CONFIG',
                'USERVER_RPS_CCONTROL_ENABLED',
                'MORE_CONFIGS',
                'POSTGRES_CONNECTION_POOL_SETTINGS',
            ],
            'my-custom-service',
            {
                'CUSTOM_CONFIG': {'config': False},
                'USERVER_RPS_CCONTROL_ENABLED': True,
                'POSTGRES_CONNECTION_POOL_SETTINGS': {
                    '__default__': {
                        'min_pool_size': 10,
                        'max_pool_size': 30,
                        'max_queue_size': 100,
                    },
                },
            },
            {
                'USERVER_RPS_CCONTROL_ENABLED': False,
                'POSTGRES_CONNECTION_POOL_SETTINGS': {
                    '__default__': {
                        'min_pool_size': 4,
                        'max_pool_size': 15,
                        'max_queue_size': 200,
                    },
                },
            },
            id='remove redefinitions configs',
        ),
    ],
)
@pytest.mark.pgsql(
    'uservice_dynconf',
    files=['default_configs.sql', 'custom_configs.sql'],
)
async def test_configs_delete_values(
        service_client,
        check_configs_state,
        mocked_time,
        ids,
        service,
        configs,
        expected,
):
    await check_configs_state(
        ids=ids,
        service=service,
        expected_configs=configs,
        expected_kill_switches_enabled=[],
        expected_kill_switches_disabled=[]
    )

    response = await service_client.post(
        '/admin/v1/configs/delete', json={'service': service, 'ids': ids},
    )

    assert response.status_code == 204

    await service_client.invalidate_caches(cache_names=['configs-cache'])
    await check_configs_state(
        ids=ids,
        service=service,
        expected_configs=expected,
        expected_kill_switches_enabled=[],
        expected_kill_switches_disabled=[]
    )


@pytest.mark.parametrize(
    'ids, configs, kill_switches_enabled, kill_switches_disabled',
    [
        pytest.param(
            ['SAMPLE_ENABLED_KILL_SWITCH'],
            {'SAMPLE_ENABLED_KILL_SWITCH': 1},
            ['SAMPLE_ENABLED_KILL_SWITCH'],
            [],
            id='delete enabled kill switch',
        ),
        pytest.param(
            ['SAMPLE_DISABLED_KILL_SWITCH'],
            {'SAMPLE_DISABLED_KILL_SWITCH': 2},
            [],
            ['SAMPLE_DISABLED_KILL_SWITCH'],
            id='delete disabled kill switch',
        ),
        pytest.param(
            ['SAMPLE_ENABLED_KILL_SWITCH', 'SAMPLE_DISABLED_KILL_SWITCH'],
            {
                'SAMPLE_ENABLED_KILL_SWITCH': 1,
                'SAMPLE_DISABLED_KILL_SWITCH': 2,
            },
            ['SAMPLE_ENABLED_KILL_SWITCH'],
            ['SAMPLE_DISABLED_KILL_SWITCH'],
            id='delete enabled and disabled kill switches',
        ),
    ],
)
@pytest.mark.pgsql(
    'uservice_dynconf',
    files=['kill_switches.sql'],
)
async def test_remove_kill_switches(
    service_client, check_configs_state,
    ids, configs, kill_switches_enabled, kill_switches_disabled,
):
    service = 'service-with-kill-switches'
    await check_configs_state(
        ids=ids,
        service=service,
        expected_configs=configs,
        expected_kill_switches_enabled=kill_switches_enabled,
        expected_kill_switches_disabled=kill_switches_disabled
    )

    response = await service_client.post(
        '/admin/v1/configs/delete', json={
            'service': service, 'ids': ids
        },
    )

    assert response.status_code == 204

    await service_client.invalidate_caches(cache_names=['configs-cache'])
    await check_configs_state(
        ids=ids,
        service=service,
        expected_configs={},
        expected_kill_switches_enabled=[],
        expected_kill_switches_disabled=[]
    )


@pytest.mark.parametrize(
    'request_data',
    [
        ({}),
        ({'ids': []}),
        ({'ids': ['ID']}),
        ({'service': ''}),
        ({'service': 'my-service'}),
        ({'ids': ['ID'], 'service': ''}),
        ({'ids': [], 'service': 'my-service'}),
    ],
)
async def test_remove_configs_400(
        service_client, mocked_time, request_data,
):
    response = await service_client.post(
        '/admin/v1/configs/delete', json=request_data,
    )
    assert response.status_code == 400
    assert response.json() == {
        'code': '400',
        'message': 'Fields \'ids\' and \'service\' are required',
    }
