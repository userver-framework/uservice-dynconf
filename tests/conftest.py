import pathlib
import typing

import pytest

from testsuite.daemons import service_client
from testsuite import utils

from testsuite.databases.pgsql import discover

pytest_plugins = [
    'pytest_userver.plugins',
    'testsuite.databases.pgsql.pytest_plugin',
]

USERVER_CONFIG_HOOKS = [
    'userver_config_update_service',
]


@pytest.fixture(scope='session')
def root_dir():
    """Path to root directory service."""
    return pathlib.Path(__file__).parent.parent


@pytest.fixture(scope='session')
def initial_data_path(
        root_dir,
) -> typing.List[pathlib.Path]:
    """Path for find files with data"""
    return [
        root_dir / 'postgresql/data',
    ]


@pytest.fixture(scope='session')
def pgsql_local(root_dir, pgsql_local_create):
    """Create schemas databases for tests"""
    databases = discover.find_schemas(
        'uservice_dynconf',
        [root_dir.joinpath('postgresql/schemas')],
    )
    return pgsql_local_create(list(databases.values()))


@pytest.fixture
def client_deps(pgsql):
    pass


@pytest.fixture(scope='session')
def userver_config_update_service(pytestconfig, service_port):
    def _patch_config(config_yaml, config_vars):
        components = config_yaml['components_manager']['components']
        updater = components['dynamic-config-client-updater']
        component['config-url'] = 'localhost:' + str(service_port)

    return _patch_config
