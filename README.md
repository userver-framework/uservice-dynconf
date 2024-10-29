# uservice-dynconf
[![CI](https://github.com/userver-framework/uservice-dynconf/actions/workflows/ci.yml/badge.svg)](https://github.com/userver-framework/uservice-dynconf/actions/workflows/ci.yml)

The service to control [dynamic configs](https://userver.tech/d5/d46/md_en_2userver_2dynamic__config.html) of the other userver-based services.

Service using postgres db version 10+

The service is ready to use. Web interface for configs administration would be added soon.

## Run uservice-dynconf

```sh
# Clone the service
git clone git@github.com:userver-framework/uservice-dynconf.git
cd uservice-dynconf

# Create schemas and struct db for storage configs
psql -f ./postgresql/schemas/uservice_dynconf.sql

# Full db default value configs
psql -f ./postgresql/data/default_configs.sql

# Run build or install (optional Env PREFIX set install dir)
make install

# Run service
uservice-dynconf --config /etc/uservice-dynconf/config_vars.yaml
```

## HowTo configure other userver-based microservices to use uservice-dynconf

In [file `static_configs.yaml`](https://github.com/userver-framework/service_template/blob/develop/configs/static_config.yaml#L26) of the microservice add the following lines to connect and retrieve configurations from the `uservice-dynconf`:
```yaml
        dynamic-config-client:
            # uservice-dynconf address
            config-url: http://localhost:8083
            # service name to send to uservice-dynconf
            service-name: microservice-name
            http-retries: 5
            http-timeout: 20s
        dynamic-config-client-updater:
            config-settings: false
            full-update-interval: 1m
            load-only-my-values: true
            store-enabled: true
            update-interval: 5s
```

## HTTP API

HTTP REST API of the uservice-dynconf is described in OpenAPI format in [file](https://github.com/userver-framework/uservice-dynconf/blob/develop/docs/api/api.yaml)

## Makefile

Makefile contains useful targets for development:

* `make build-debug` - debug build of the service with all the assertions and sanitizers enabled
* `make build-release` - release build of the service with LTO
* `make test-debug` - does a `make build-debug` and runs all the tests on the result
* `make test-release` - does a `make build-release` and runs all the tests on the result
* `make service-start-debug` - builds the service in debug mode and starts it
* `make service-start-release` - builds the service in release mode and starts it
* `make` or `make all` - builds and runs all the tests in release and debug modes
* `make format` - autoformat all the C++ and Python sources
* `make clean-` - cleans the object files
* `make dist-clean` - clean all, including the CMake cached configurations
* `make install` - does a `make build-release` and runs install in directory set in environment `PREFIX`
* `make install-debug` - does a `make build-debug` and runs install in directory set in environment `PREFIX`
* `make docker-COMMAND` - run `make COMMAND` in docker environment
* `make docker-build-debug` - debug build of the service with all the assertions and sanitizers enabled in docker environment
* `make docker-test-debug` - does a `make build-debug` and runs all the tests on the result in docker environment
* `make docker-start-service-release` - does a `make install-release` and runs service in docker environment
* `make docker-start-service-debug` - does a `make install-debug` and runs service in docker environment
* `make docker-clean-data` - stop docker containers and clean database data

Edit `Makefile.local` to change the default configuration and build options.

# License

Distributed under the [Apache-2.0 License](https://github.com/userver-framework/userver-dynconf/blob/develop/LICENSE) with the [CLA](https://github.com/userver-framework/userver-dynconf/blob/develop/CONTRIBUTING.md).
