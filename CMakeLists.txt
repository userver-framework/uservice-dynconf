cmake_minimum_required(VERSION 3.12)
project(uservice-dynconf CXX)

include(third_party/userver/cmake/SetupEnvironment.cmake)

add_subdirectory(third_party/userver)


add_executable(${PROJECT_NAME}
	src/models/config.hpp
	src/utils/make_error.hpp
	src/utils/make_error.cpp
	src/cache/configs_cache.hpp
	src/cache/configs_cache.cpp
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

IF(DEFINED ENV{PREFIX})
	MESSAGE(STATUS "Set install prefix: $ENV{PREFIX}")
	file(TO_CMAKE_PATH "$ENV{PREFIX}" PREFIX_PATH)
	set(CMAKE_INSTALL_PREFIX ${PREFIX_PATH})
ENDIF()

set(CONFIG_VAR_PATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME}/config_vars.yaml)
set(CONFIG_FALLBACK_PATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME}/dynamic_config_fallback.json)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configs/static_config.yaml.in ${CMAKE_CURRENT_SOURCE_DIR}/configs/static_config.yaml)

FILE(GLOB CONFIGS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.yaml)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CONFIGS_FILES} DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME})
