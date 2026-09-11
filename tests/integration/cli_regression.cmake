file(REMOVE_RECURSE "${TEST_DIR}")
file(MAKE_DIRECTORY "${TEST_DIR}")
file(WRITE "${TEST_DIR}/first game.rom" "fixture")
file(WRITE "${TEST_DIR}/second game.rom" "fixture")
file(WRITE "${TEST_DIR}/emulator.exe" "fixture")

function(run_cli expected)
	execute_process(
		COMMAND "${PROGRAM}" ${ARGN}
		WORKING_DIRECTORY "${TEST_DIR}"
		RESULT_VARIABLE result
		OUTPUT_VARIABLE output
		ERROR_VARIABLE error
		TIMEOUT 10
	)
	if(expected STREQUAL "success" AND NOT result STREQUAL "0")
		message(FATAL_ERROR "CLI failed: ${ARGN}\n${result}\n${output}\n${error}")
	elseif(expected STREQUAL "failure" AND result STREQUAL "0")
		message(FATAL_ERROR "CLI accepted invalid input: ${ARGN}")
	endif()
	set(CLI_OUTPUT "${output}" PARENT_SCOPE)
endfunction()

run_cli(success --help)
foreach(command add-game list-game add-emu list-emu add-profile list-profiles
		launch replace-game replace-emu list-history list-strategies)
	if(NOT CLI_OUTPUT MATCHES "${command}")
		message(FATAL_ERROR "Missing command: ${command}")
	endif()
endforeach()

run_cli(failure add-game)
run_cli(failure unknown-command)
run_cli(failure add-game --name missing --path "${TEST_DIR}/missing.rom")

run_cli(success add-game --name Game --platform console
	--path "${TEST_DIR}/first game.rom")
if(NOT EXISTS "${TEST_DIR}/config/emulators.db")
	message(FATAL_ERROR "SQLite database was not created")
endif()
file(READ "${TEST_DIR}/config/catalog.json" catalog)
string(JSON game_id GET "${catalog}" games 0 id)

run_cli(success add-game --name Game --platform console
	--path "${TEST_DIR}/second game.rom" --repl)
file(READ "${TEST_DIR}/config/catalog.json" catalog)
string(JSON saved_path GET "${catalog}" games 0 content_path)
if(NOT saved_path STREQUAL "${TEST_DIR}/second game.rom")
	message(FATAL_ERROR "Game update was not persisted")
endif()

run_cli(success add-emu --name Emulator --platform console
	--path "${TEST_DIR}/emulator.exe" --arg=--base)
file(READ "${TEST_DIR}/config/catalog.json" catalog)
string(JSON emulator_id GET "${catalog}" emulators 0 id)

run_cli(success add-profile --name Default --game-id "${game_id}"
	--emulator-id "${emulator_id}" --strategy-id direct --arg=--profile
	--working-directory "${TEST_DIR}" --default)
file(READ "${TEST_DIR}/config/catalog.json" catalog)
string(JSON profile_id GET "${catalog}" launch_profiles 0 id)
string(JSON is_default GET "${catalog}" launch_profiles 0 is_default)
if(NOT is_default)
	message(FATAL_ERROR "Default launch profile was not persisted")
endif()

run_cli(success list-profiles --game-id "${game_id}")
if(NOT CLI_OUTPUT MATCHES "${profile_id}")
	message(FATAL_ERROR "Launch profile is missing from output")
endif()
run_cli(success list-game)
if(NOT CLI_OUTPUT MATCHES "${game_id}")
	message(FATAL_ERROR "Game id is missing from output")
endif()
run_cli(success list-emu)
if(NOT CLI_OUTPUT MATCHES "${emulator_id}")
	message(FATAL_ERROR "Emulator id is missing from output")
endif()
run_cli(success replace-game --old_name Game --name Renamed --platform console
	--path "${TEST_DIR}/second game.rom")
run_cli(success list-history)
if(NOT CLI_OUTPUT MATCHES "list-history")
	message(FATAL_ERROR "History should include the current command")
endif()
