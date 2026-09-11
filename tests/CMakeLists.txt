include(GoogleTest)

function(add_emu_test target source)
	add_executable("${target}" "${source}")
	target_include_directories("${target}" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
	target_link_libraries("${target}" PRIVATE GTest::gtest_main ${ARGN})
	gtest_discover_tests("${target}")
endfunction()

add_emu_test(game_service_test unit/game_service_test.cpp emu_application)
add_emu_test(emulator_service_test unit/emulator_service_test.cpp
	emu_application)
add_emu_test(id_generator_test unit/id_generator_test.cpp emu_application)
add_emu_test(launch_profile_service_test unit/launch_profile_service_test.cpp
	emu_application)
add_emu_test(launch_game_service_test unit/launch_game_service_test.cpp
	emu_application emu_launch)
add_emu_test(launch_strategy_registry_test unit/launch_strategy_registry_test.cpp
	emu_application)
add_emu_test(direct_strategy_test unit/direct_strategy_test.cpp emu_launch)
add_emu_test(native_process_runner_test
	integration/native_process_runner_test.cpp emu_launch)

if(EMU_BUILD_PERSISTENCE)
	add_emu_test(sqlite_repositories_test
		integration/sqlite_repositories_test.cpp emu_persistence)
	add_emu_test(config_test integration/config_test.cpp emu_persistence
		nlohmann_json::nlohmann_json)
	add_emu_test(json_catalog_transfer_test
		integration/json_catalog_transfer_test.cpp emu_persistence
		nlohmann_json::nlohmann_json)
endif()

if(EMU_BUILD_CLI)
	add_test(NAME cli_regression
		COMMAND "${CMAKE_COMMAND}"
			"-DPROGRAM=$<TARGET_FILE:emu_manager>"
			"-DTEST_DIR=${CMAKE_CURRENT_BINARY_DIR}/cli-data"
			-P "${CMAKE_CURRENT_SOURCE_DIR}/integration/cli_regression.cmake"
	)
endif()

set(core_headers
	domain/entity_ids
	domain/game
	domain/emulator
	domain/launch_profile
	domain/command_history_entry
	application/application_error
	application/game_repository
	application/emulator_repository
	application/launch_profile_repository
	application/history_repository
	application/id_generator
	application/game_service
	application/emulator_service
	application/launch_profile_service
	application/process_runner
	application/launch_context
	application/launch_strategy_info
	application/launch_strategy
	application/launch_strategy_registry
	application/launch_game_service
)

foreach(header IN LISTS core_headers)
	string(REPLACE "/" "_" name "${header}")
	set(source "${CMAKE_CURRENT_BINARY_DIR}/${name}_header.cpp")
	file(WRITE "${source}" "#include \"emu/${header}.h\"\n")
	add_library("${name}_header" OBJECT "${source}")
	target_link_libraries("${name}_header" PRIVATE emu_domain)
endforeach()
