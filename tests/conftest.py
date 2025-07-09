import pathlib
import typing

import pytest

from testsuite.databases.pgsql import discover

pytest_plugins = ['pytest_userver.plugins.postgresql']
USERVER_CONFIG_HOOKS = ['static_config_hook']


@pytest.fixture(scope='session')
def static_config_hook(service_source_dir):
    def _patch_config(config_yaml, config_vars):
        components = config_yaml['components_manager']['components']
        assert 'resources-cache' in components
        components['resources-cache']['dir'] = str(
            pathlib.Path(service_source_dir).joinpath('www-data'),
        )

    return _patch_config


@pytest.fixture(scope='session')
def service_source_dir():
    return pathlib.Path(__file__).parent.parent


@pytest.fixture(scope='session')
def initial_data_path(
        service_source_dir,
) -> typing.List[pathlib.Path]:
    """Path for find files with data"""
    return [
        service_source_dir / 'postgresql/data',
        service_source_dir,
    ]


@pytest.fixture(scope='session')
def pgsql_local(service_source_dir, pgsql_local_create):
    """Create schemas databases for tests"""
    databases = discover.find_schemas(
        'testdb',
        [service_source_dir.joinpath('postgresql/schemas')],
    )
    return pgsql_local_create(list(databases.values()))


@pytest.fixture
async def check_configs_state(service_client):
    """
    Check that configs state (values and modes) is equal to expected
    by sending request to /configs/values
    """
    async def check(
            ids: list[str], service: str, expected_configs: dict,
            expected_kill_switches_enabled: list[str],
            expected_kill_switches_disabled: list[str],
    ) -> bool:
        response = await service_client.post(
            '/configs/values', json={'ids': ids, 'service': service},
        )
        assert response.status_code == 200
        json = response.json()
        assert json['configs'] == expected_configs
        assert json.get('kill_switches_enabled', []
                        ) == expected_kill_switches_enabled
        assert json.get('kill_switches_disabled', []
                        ) == expected_kill_switches_disabled

    return check
