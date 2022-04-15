cmake_minimum_required(VERSION 3.12)
project(service_dynamic_configs CXX)

include(third_party/userver/cmake/SetupEnvironment.cmake)

add_subdirectory(third_party/userver)


add_executable(${PROJECT_NAME}
	src/models/config.hpp
	src/utils/make_error.hpp
	src/utils/make_error.cpp
	src/cache/settings_cache.hpp
	src/cache/settings_cache.cpp
	src/handlers/admin_v1_configs.hpp
	src/handlers/admin_v1_configs.cpp
	src/handlers/configs_values.hpp
	src/handlers/configs_values.cpp
	src/handlers/admin_v1_configs_delete.hpp
	src/handlers/admin_v1_configs_delete.cpp
	src/sql/sql_query.hpp
    src/main.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME} userver-core userver-postgresql)

add_subdirectory(tests)
