# uservice-dynconf
The service to control [dynamic configs](https://userver-framework.github.io/dd/d2c/md_en_schemas_dynamic_configs.html) of the other userver-based services.

The service is ready to use. Web interface for configs administration would be added soon.

## Run uservice-config

To create your own userver-based service follow the following steps:

1. Clone the service `git clone git@github.com:userver-framework/uservice-dynconf.git && cd uservice-dynconf && git submodule update --init`
2. Run postgresql (support Postgres-10+)
3. Create db and run sql file ./postgresql/schemas/uservice_dynconf.sql - this file create schemas and struct db for storage configs
4. Run sql file ./postgresql/data/default_configs.sql - this file full your db default configs for service based userver framework
5. Run `PREFIX=/usr/local make install`
6. Setting db parameters in `/usr/local/etc/uservice-dynconf/config_vars.yaml`
7. Run `/usr/local/bin/uservice-dynconf --config /usr/local/etc/uservice-dynconf/config_vars.yaml`

## Using this another userver microservices

In `file static_configs.yaml` set:
```yaml
components_manager:
    components:
        taxi-configs-client:
            config-url: http://localhost:8083
            http-retries: 5
            http-timeout: 20s
            service-name: microservice-name
        taxi-config-client-updater:
            config-settings: false
            fallback-path: dynamic_config_fallback.json
            full-update-interval: 1m
            load-only-my-values: true
            store-enabled: true
            update-interval: 5s
```

## Http api

Http api uservice-dynconf described in OpenAPI format in [file](https://github.com/userver-framework/uservice-dynconf/blob/develop/docs/api/api.yaml)

## Makefile

Makefile contains typicaly useful targets for development:

* `make build-debug` - debug build of the service with all the assertions and sanitizers enabled
* `make build-release` - release build of the service with LTO
* `make test-debug` - does a `make build-debug` and runs all the tests on the result
* `make test-release` - does a `make build-release` and runs all the tests on the result
* `make` or `make all` - builds and runs all the tests in release and debug modes
* `make format` - autoformat all the C++ and Python sources
* `make clean-` - cleans the object files
* `make dist-clean` - clean all, including the CMake cached configurations

Edit `Makefile.local` to change the default configuration and build options.

# License

The original template is distributed under the [Apache-2.0 License](https://github.com/userver-framework/userver/blob/develop/LICENSE) and [CLA](https://github.com/userver-framework/userver/blob/develop/CONTRIBUTING.md). Services based on the template may change the license and CLA.
