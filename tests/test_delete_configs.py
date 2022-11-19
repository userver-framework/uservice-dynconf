import pytest

from testsuite.databases import pgsql


# @pytest.mark.parametrize(
#     'ids, service, configs, expected',
#     [
#         pytest.param(
#             ['CUSTOM_CONFIG'],
#             'my-custom-service',
#             {'CUSTOM_CONFIG': {'config': False}},
#             {},
#             id='delete one config',
#         ),
#         pytest.param(
#             ['CUSTOM_CONFIG', 'ADD_CONFIG', 'MORE_CONFIGS'],
#             'my-custom-service',
#             {'CUSTOM_CONFIG': {'config': False}},
#             {},
#             id='remove bulk configs',
#         ),
#         pytest.param(
#             [
#                 'CUSTOM_CONFIG',
#                 'USERVER_RPS_CCONTROL_ENABLED',
#                 'MORE_CONFIGS',
#                 'POSTGRES_CONNECTION_POOL_SETTINGS',
#             ],
#             'my-custom-service',
#             {
#                 'CUSTOM_CONFIG': {'config': False},
#                 'USERVER_RPS_CCONTROL_ENABLED': True,
#                 'POSTGRES_CONNECTION_POOL_SETTINGS': {
#                     '__default__': {
#                         'min_pool_size': 10,
#                         'max_pool_size': 30,
#                         'max_queue_size': 100,
#                     },
#                 },
#             },
#             {
#                 'USERVER_RPS_CCONTROL_ENABLED': False,
#                 'POSTGRES_CONNECTION_POOL_SETTINGS': {
#                     '__default__': {
#                         'min_pool_size': 4,
#                         'max_pool_size': 15,
#                         'max_queue_size': 200,
#                     },
#                 },
#             },
#             id='remove redefinitions configs',
#         ),
#     ],
# )
# @pytest.mark.pgsql(
#     'uservice_dynconf',
#     files=['default_configs.sql', 'custom_configs.sql'],
# )
# async def test_configs_delete_values(
#         service_client,
#         mocked_time,
#         ids,
#         service,
#         configs,
#         expected,
# ):
#     response = await service_client.post(
#         '/configs/values', json={'ids': ids, 'service': service},
#     )
#     assert response.status_code == 200
#     assert response.json()['configs'] == configs

#     response = await service_client.post(
#         '/admin/v1/configs/delete', json={'service': service, 'ids': ids},
#     )

#     response.status_code == 204

#     await service_client.invalidate_caches()
#     response = await service_client.post(
#         '/configs/values', json={'ids': ids, 'service': service},
#     )
#     assert response.status_code == 200
#     assert response.json()['configs'] == expected


# @pytest.mark.parametrize(
#     'request_data',
#     [
#         ({}),
#         ({'ids': []}),
#         ({'ids': ['ID']}),
#         ({'service': ''}),
#         ({'service': 'my-service'}),
#         ({'ids': ['ID'], 'service': ''}),
#         ({'ids': [], 'service': 'my-service'}),
#     ],
# )
# async def test_remove_configs_400(
#         service_client, mocked_time, request_data,
# ):
#     response = await service_client.post(
#         '/admin/v1/configs/delete', json=request_data,
#     )
#     assert response.status_code == 400
#     assert response.json() == {
#         'code': '400',
#         'message': 'Fields \'ids\' and \'service\' are required',
#     }
