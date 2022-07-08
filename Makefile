CMAKE_COMMON_FLAGS ?= -DUSERVER_OPEN_SOURCE_BUILD=1 -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
CMAKE_DEBUG_FLAGS ?= -DUSERVER_SANITIZE='addr ub'
CMAKE_RELESEAZE_FLAGS ?=
CMAKE_OS_FLAGS ?= -DUSERVER_FEATURE_CRYPTOPP_BLAKE2=0 -DUSERVER_FEATURE_REDIS_HI_MALLOC=1
NPROCS ?= $(shell nproc)

# NOTE: use Makefile.local for customization
-include Makefile.local

# Debug cmake configuration
build_debug/Makefile:
	@mkdir -p build_debug
	@cd build_debug && \
      cmake -DCMAKE_BUILD_TYPE=Debug $(CMAKE_COMMON_FLAGS) $(CMAKE_DEBUG_FLAGS) $(CMAKE_OS_FLAGS) $(CMAKE_OPTIONS) ..

# Release cmake configuration
build_release/Makefile:
	@mkdir -p build_release
	@cd build_release && \
      cmake -DCMAKE_BUILD_TYPE=Release $(CMAKE_COMMON_FLAGS) $(CMAKE_RELESEAZE_FLAGS) $(CMAKE_OS_FLAGS) $(CMAKE_OPTIONS) ..

# build using cmake
build-impl-%: build_%/Makefile
	@cmake --build build_$* -j$(NPROCS) --target uservice-dynconf

# test
test-impl-%: build-impl-%
	@cd build_$* && ctest -V
	@pep8 tests

# clean
clean-impl-%:
	@cd build_$* && $(MAKE) clean
	@rm -f ./configs/static_config.yaml

# dist-clean
.PHONY: dist-clean
dist-clean:
	@rm -rf build_*
	@rm -f ./configs/static_config.yaml

# format
.PHONY: format
format:
	@find src -name '*pp' -type f | xargs clang-format -i
	@find tests -name '*.py' -type f | xargs autopep8 -i

.PHONY: cmake-debug build-debug test-debug clean-debug cmake-release build-release test-release clean-release install install-debug

install-debug: build-debug
	@cd build_debug && \
		cmake --install . -v --component uservice-dynconf

install: build-release
	@cd build_release && \
		cmake --install . -v --component uservice-dynconf

# Hide target, use only in docker environment
--debug-start-in-docker: install
	@sed -i 's/config_vars.yaml/config_vars.docker.yaml/g' /home/user/.local/etc/uservice-dynconf/static_config.yaml
	@psql 'postgresql://uservice_dynconf:password@uservice-dynconf-postgres:5432/uservice_dynconf' -f ./postgresql/data/default_configs.sql
	@/home/user/.local/bin/uservice-dynconf \
		--config /home/user/.local/etc/uservice-dynconf/static_config.yaml

# Hide target, use only in docker environment
--debug-start-in-docker-debug: install-debug
	@sed -i 's/config_vars.yaml/config_vars.docker.yaml/g' /home/user/.local/etc/uservice-dynconf/static_config.yaml
	@psql 'postgresql://uservice_dynconf:password@uservice-dynconf-postgres:5432/uservice_dynconf' -f ./postgresql/data/default_configs.sql
	@/home/user/.local/bin/uservice-dynconf \
		--config /home/user/.local/etc/uservice-dynconf/static_config.yaml

.PHONY: docker-cmake-debug docker-build-debug docker-test-debug docker-clean-debug docker-cmake-release docker-build-release docker-test-release docker-clean-release docker-install docker-install-debug docker-start-service-debug docker-start-service docker-clean-data 

# Build and runs service in docker environment
docker-start-service-debug:
	@rm -f ./configs/static_config.yaml
	@docker-compose run -p 8083:8083 --rm uservice-dynconf make -- --debug-start-in-docker-debug

# Build and runs service in docker environment
docker-start-service:
	@rm -f ./configs/static_config.yaml
	@docker-compose run -p 8083:8083 --rm uservice-dynconf make -- --debug-start-in-docker

# Stop docker container and remove PG data
docker-clean-data:
	@docker-compose down -v
	@rm -rf ./.pgdata

# Start targets makefile in docker enviroment
docker-%:
	docker-compose run --rm uservice-dynconf make $*

# Explicitly specifying the targets to help shell with completitions
cmake-debug: build_debug/Makefile
cmake-release: build_release/Makefile

build-debug: build-impl-debug
build-release: build-impl-release

test-debug: test-impl-debug
test-release: test-impl-release

clean-debug: clean-impl-debug
clean-release: clean-impl-release
