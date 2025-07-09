import pytest


@pytest.mark.servicetest
@pytest.mark.pgsql('uservice_dynconf', files=['default_configs.sql'])
async def test_service_default_start(service_client, service_baseurl):
    delimiter = '=' * 100
    message = f'\n{delimiter}\nStarted service at {service_baseurl}'
    message += f'\n{delimiter}\n'
    print(message)
