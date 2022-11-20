cmake_minimum_required(VERSION 3.12)
project(uservice-dynconf CXX)

include(third_party/userver/cmake/SetupEnvironment.cmake)
include(GNUInstallDirs)

add_subdirectory(third_party/userver)


add_executable(${PROJECT_NAME}
	src/models/service.hpp
	src/models/config.hpp
	src/utils/make_error.hpp
	src/utils/make_error.cpp
	src/cache/configs/configs_cache.hpp
	src/cache/configs/configs_cache.cpp
	src/cache/services/services_cache.hpp
	src/cache/services/services_cache.cpp
	src/auth/auth.cpp
	src/handlers/post-configs_values/view.hpp
	src/handlers/post-configs_values/view.cpp
	src/handlers/admin/v1/post-configs_delete/view.hpp
	src/handlers/admin/v1/post-configs_delete/view.cpp
	src/handlers/admin/v1/post-configs/view.hpp
	src/handlers/admin/v1/post-configs/view.cpp
	src/handlers/admin/v2/get-configs_uuid/view.hpp
	src/handlers/admin/v2/get-configs_uuid/view.cpp
	src/handlers/admin/v2/patch-configs_uuid/view.hpp
	src/handlers/admin/v2/patch-configs_uuid/view.cpp
	src/handlers/admin/v2/delete-configs_uuid/view.hpp
	src/handlers/admin/v2/delete-configs_uuid/view.cpp
	src/handlers/admin/v2/get-configs/view.hpp
	src/handlers/admin/v2/get-configs/view.cpp
	src/handlers/admin/v2/get-services/view.hpp
	src/handlers/admin/v2/get-services/view.cpp
	src/handlers/admin/v2/post-configs/view.hpp
	src/handlers/admin/v2/post-configs/view.cpp
	src/handlers/admin/v2/post-configs_uuid_clone/view.hpp
	src/handlers/admin/v2//post-configs_uuid_clone/view.cpp
	src/handlers/admin/v2//post-services_uuid_clone/view.hpp
	src/handlers/admin/v2/post-services_uuid_clone/view.cpp
	src/sql/sql_query.hpp
    src/main.cpp)

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

FILE(GLOB CONFIGS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.yaml ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.json)


install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${PROJECT_NAME})
install(FILES ${CONFIGS_FILES} DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME} COMPONENT ${PROJECT_NAME})
