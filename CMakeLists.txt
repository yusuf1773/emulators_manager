cmake_minimum_required(VERSION 3.20)
project(emu_manager LANGUAGES C CXX)
set(CMAKE_CXX_EXTENSIONS OFF)

option(EMU_BUILD_CLI "Build the command-line application" ON)
option(EMU_BUILD_PERSISTENCE "Build SQLite persistence and JSON transfer" ON)

if(EMU_BUILD_CLI AND NOT EMU_BUILD_PERSISTENCE)
	message(FATAL_ERROR "The CLI requires EMU_BUILD_PERSISTENCE=ON")
endif()

include(CTest)
include(cmake/dependencies.cmake)

add_library(emu_domain INTERFACE)
target_compile_features(emu_domain INTERFACE cxx_std_23)
target_include_directories(emu_domain INTERFACE "${PROJECT_SOURCE_DIR}/include")

add_library(emu_application
	src/application/emulator_service.cpp
	src/application/game_service.cpp
	src/application/id_generator.cpp
	src/application/launch_game_service.cpp
	src/application/launch_profile_service.cpp
	src/application/launch_strategy_registry.cpp
)
target_link_libraries(emu_application PUBLIC emu_domain)

add_library(emu_launch
	src/launch/direct_strategy.cpp
	src/launch/native_process_runner.cpp
)
target_link_libraries(emu_launch PUBLIC emu_application)

if(EMU_BUILD_PERSISTENCE)
	add_library(emu_persistence
		src/persistence/config.cpp
		src/persistence/json_catalog_transfer.cpp
		src/persistence/sqlite_database.cpp
		src/persistence/sqlite_repositories.cpp
	)
	target_link_libraries(emu_persistence PUBLIC
		emu_application
		SQLite::SQLite3
	)
	target_link_libraries(emu_persistence PRIVATE
		nlohmann_json::nlohmann_json
	)
endif()

if(EMU_BUILD_CLI)
	add_subdirectory(app)
endif()

if(BUILD_TESTING)
	add_subdirectory(tests)
endif()
