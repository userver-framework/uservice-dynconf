import pytest

from testsuite.databases import pgsql


# Make sure that the defaults in default_configs.sql match values in
# configs/dynamic_config_fallback.json and there's no additional values in the
# latter.
@pytest.mark.pgsql('uservice_dynconf', files=['default_configs.sql'])
async def test_default_values(pgsql, load_json):
    cursor = pgsql['uservice_dynconf'].cursor()
    cursor.execute(
        'SELECT json_object_agg(config_name, config_value) '
        'FROM uservice_dynconf.configs WHERE service_uuid=\'__default_uuid__\'',
    )
    data = cursor.fetchall()
    assert len(data) == 1
    assert len(data[0]) == 1
    db_defaults = data[0][0]

    service_defaults = load_json('configs/dynamic_config_fallback.json')

    for key, value in service_defaults.items():
        if key != 'USERVER_FILES_CONTENT_TYPE_MAP':
             assert key in db_defaults
             assert db_defaults[key] == value, 'At key: ' + key
