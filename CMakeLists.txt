cmake_minimum_required(VERSION 3.12)
project(service_dynamic_configs CXX)

include(third_party/userver/cmake/SetupEnvironment.cmake)
#include(third_party/userver/cmake/UserverTestsuite.cmake)
#include(third_party/userver/testsuite/SetupUserverTestsuiteEnv.cmake)

add_subdirectory(third_party/userver)


add_executable(${PROJECT_NAME}
	src/models/config.hpp
	src/cache/settings_cache.hpp
	src/cache/settings_cache.cpp
	src/components/pg_init.hpp
	src/components/pg_init.cpp
	src/sql/sql_query.hpp
    src/hello.hpp
    src/hello.cpp
    src/main.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME} userver-core userver-postgresql)

add_subdirectory(tests)

#userver_testsuite_add(
#   NAME testsuite-${PROJECT_NAME}
#  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests
#  PYTHON_BINARY /usr/local/bin/python3
#  PYTEST_ARGS
#  --service-name=${PROJECT_NAME}
#  --service-config=${CMAKE_CURRENT_SOURCE_DIR}/configs/static_config.yaml
#  --service-source-dir=${CMAKE_CURRENT_SOURCE_DIR}
#  --service-binary=${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}
#  PYTHONPATH
#   ${CMAKE_SOURCE_DIR}/testsuite/pytest_plugins
#)

