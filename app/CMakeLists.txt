add_executable(emu_manager
	main.cpp
	bootstrap/application.cpp
	bootstrap/app_context.cpp
	cli/command_manager.cpp
	cli/commands/cli_command.cpp
	cli/commands/add_game_command.cpp
	cli/commands/add_emu_command.cpp
	cli/commands/add_launch_profile_command.cpp
	cli/commands/list_games_command.cpp
	cli/commands/list_emus_command.cpp
	cli/commands/list_launch_strategies_command.cpp
	cli/commands/list_launch_profiles_command.cpp
	cli/commands/list_command_history_command.cpp
	cli/commands/launch_game_command.cpp
	cli/commands/replace_game_command.cpp
	cli/commands/replace_emu_command.cpp
)

target_include_directories(emu_manager PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(emu_manager PRIVATE
	emu_application
	emu_launch
	emu_persistence
	CLI11::CLI11
)

set_target_properties(emu_manager PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)

if(MSVC)
	target_compile_options(emu_manager PRIVATE /utf-8)
endif()
