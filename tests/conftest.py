import pathlib

import pytest

from testsuite.daemons import service_client
from testsuite import utils

from testsuite.databases.pgsql import discover

pytest_plugins = [
    'testsuite.pytest_plugin',
    'testsuite.databases.pgsql.pytest_plugin',
]

# install it using `pip3 install yandex-taxi-testsuite`
SERVICE_NAME = 'service_dynamic_configs'
SERVICE_BASEURL = 'http://localhost:8083/'
ROOT_PATH = pathlib.Path(__file__).parent.parent


def pytest_addoption(parser) -> None:
    group = parser.getgroup('userver')
    group.addoption(
        '--build-dir',
        type=pathlib.Path,
        help='Path to service build directory.',
    )


@pytest.fixture
async def service_dynamic_configs_client(
        service_dynamic_configs_daemon,
        service_client_options,
        ensure_daemon_started,
        mockserver,
        pgsql,
        pgsql_local,
):
    await ensure_daemon_started(service_dynamic_configs_daemon)
    return service_client.Client(SERVICE_BASEURL, **service_client_options)


@pytest.fixture(scope='session')
def build_dir(request) -> pathlib.Path:
    return pathlib.Path(request.config.getoption('--build-dir')).resolve()


@pytest.fixture(scope='session')
async def service_dynamic_configs_daemon(
        create_daemon_scope, tmp_path_factory, build_dir,
):
    configs_path = ROOT_PATH.joinpath('configs')
    temp_dir_name = tmp_path_factory.mktemp(SERVICE_NAME)

    _copy_service_configs(
        service_name=SERVICE_NAME,
        destination=temp_dir_name,
        configs_path=configs_path,
    )

    async with create_daemon_scope(
            args=[
                str(build_dir.joinpath('service_dynamic_configs')),
                '--config',
                str(temp_dir_name.joinpath('static_config.yaml')),
            ],
            ping_url=SERVICE_BASEURL + 'ping',
    ) as scope:
        yield scope


def _copy_service_configs(
        *,
        service_name: str,
        destination: pathlib.Path,
        configs_path: pathlib.Path,
) -> None:
    for source_path in configs_path.iterdir():
        if not source_path.is_file():
            continue

        conf = source_path.read_text()
        conf = conf.replace('/etc/' + service_name, str(destination))
        conf = conf.replace('/var/cache/' + service_name, str(destination))
        conf = conf.replace('/var/log/' + service_name, str(destination))
        conf = conf.replace('/var/run/' + service_name, str(destination))
        (destination / source_path.name).write_text(conf)


@pytest.fixture(scope='session')
def example_root():
    """Path to example service root."""
    return pathlib.Path(__file__).parent.parent


@pytest.fixture(scope='session')
def pgsql_local(example_root, pgsql_local_create):
    databases = discover.find_schemas(
        'service_dynamic_configs',
        [example_root.joinpath('schemas/postgresql')],
    )
    return pgsql_local_create(list(databases.values()))


@pytest.fixture
def client_deps(pgsql):
    pass


@pytest.fixture
async def invalidate_caches(service_dynamic_configs_client, mocked_time):
    async def do_invalidate_caches():
        response = await service_dynamic_configs_client.post(
            '/tests/control',
            json={
                'mock_now': utils.timestring(mocked_time.now()),
                'invalidate_caches': {
                    'update_type': 'full',
                    'names': ['configs-cache'],
                },
            },
        )
        assert response.status_code == 200
    return do_invalidate_caches

