CMAKE_COMMON_FLAGS ?= -DUSERVER_OPEN_SOURCE_BUILD=1 -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
CMAKE_DEBUG_FLAGS ?= -DUSERVER_SANITIZE='addr ub'
CMAKE_RELEASE_FLAGS ?=
NPROCS ?= $(shell nproc)
CLANG_FORMAT ?= clang-format
DOCKER_COMPOSE ?= docker-compose

# NOTE: use Makefile.local to override the options defined above.
-include Makefile.local

CMAKE_DEBUG_FLAGS += -DCMAKE_BUILD_TYPE=Debug $(CMAKE_COMMON_FLAGS)
CMAKE_RELEASE_FLAGS += -DCMAKE_BUILD_TYPE=Release $(CMAKE_COMMON_FLAGS)

.PHONY: all
all: test-debug test-release

# Run cmake
.PHONY: cmake-debug
cmake-debug:
	git submodule update --init
	cmake -B build_debug $(CMAKE_DEBUG_FLAGS)

.PHONY: cmake-release
cmake-release:
	git submodule update --init
	cmake -B build_release $(CMAKE_RELEASE_FLAGS)

build_debug/CMakeCache.txt: cmake-debug
build_release/CMakeCache.txt: cmake-release

# Build using cmake
.PHONY: build-debug build-release
build-debug build-release: build-%: build_%/CMakeCache.txt
	cmake --build build_$* -j $(NPROCS) --target uservice-dynconf

# Test
.PHONY: test-debug test-release
test-debug test-release: test-%: build-%
	cd build_$* && ((test -t 1 && GTEST_COLOR=1 PYTEST_ADDOPTS="--color=yes" ctest -V) || ctest -V)
	pep8 tests

# Start the service (via testsuite service runner)
.PHONY: service-start-debug service-start-release
service-start-debug service-start-release: service-start-%:
	cmake --build build_$* -v --target start-uservice-dynconf

# Cleanup data
.PHONY: clean-debug clean-release
clean-debug clean-release: clean-%:
	cmake --build build_$* --target clean

.PHONY: dist-clean
dist-clean:
	rm -rf build_*
	rm -rf tests/__pycache__/
	rm -rf tests/.pytest_cache/

# Install
.PHONY: install-debug install-release
install-debug install-release: install-%: build-%
	cmake --install build_$* -v --component uservice-dynconf

.PHONY: install
install: install-release

# Format the sources
.PHONY: format
format:
	find src -name '*pp' -type f | xargs $(CLANG_FORMAT) -i
	find tests -name '*.py' -type f | xargs autopep8 -i

# Internal hidden targets that are used only in docker environment
--in-docker-start-debug --in-docker-start-release: --in-docker-start-%: install-%
	sed -i 's/config_vars.yaml/config_vars.docker.yaml/g' /home/user/.local/etc/uservice-dynconf/static_config.yaml
	psql 'postgresql://uservice_dynconf:password@uservice-dynconf-postgres:5432/uservice_dynconf' -f ./postgresql/data/default_configs.sql
	/home/user/.local/bin/uservice-dynconf \
		--config /home/user/.local/etc/uservice-dynconf/static_config.yaml
		--config_vars /home/user/.local/etc/uservice-dynconf/config_vars.yaml

# Build and run service in docker environment
.PHONY: docker-start-service-debug docker-start-service-release
docker-start-service-debug docker-start-service-release: docker-start-service-%:
	$(DOCKER_COMPOSE) run -p 8080:8080 --rm uservice-dynconf-container make -- --in-docker-start-$*

# Start targets makefile in docker environment
.PHONY: docker-cmake-debug docker-build-debug docker-test-debug docker-clean-debug docker-install-debug docker-cmake-release docker-build-release docker-test-release docker-clean-release docker-install-release
docker-cmake-debug docker-build-debug docker-test-debug docker-clean-debug docker-install-debug docker-cmake-release docker-build-release docker-test-release docker-clean-release docker-install-release: docker-%:
	$(DOCKER_COMPOSE) run --rm uservice-dynconf-container make $*

# Stop docker container and remove PG data
.PHONY: docker-clean-data
docker-clean-data:
	$(DOCKER_COMPOSE) down -v
	rm -rf ./.pgdata
