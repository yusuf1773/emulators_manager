file(REMOVE_RECURSE "${TEST_DIR}")
file(MAKE_DIRECTORY "${TEST_DIR}")
file(WRITE "${TEST_DIR}/first game.rom" "fixture")
file(WRITE "${TEST_DIR}/second game.rom" "fixture")

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
foreach(command add-game list-game add-emu list-emu launch replace-game replace-emu list-history list-strategies)
	if(NOT CLI_OUTPUT MATCHES "${command}")
		message(FATAL_ERROR "Missing command: ${command}")
	endif()
endforeach()

run_cli(success list-game)
run_cli(success list-emu)
run_cli(success list-strategies)
run_cli(failure add-game)
run_cli(failure unknown-command)
run_cli(failure add-game --name missing --path "${TEST_DIR}/missing.rom")

run_cli(success add-game --name A --path "${TEST_DIR}/first game.rom")
run_cli(success add-game --name A --path "${TEST_DIR}/second game.rom" --repl)
file(READ "${TEST_DIR}/config/games.json" games)
string(JSON saved_path GET "${games}" 0 path)
if(NOT saved_path STREQUAL "${TEST_DIR}/second game.rom")
	message(FATAL_ERROR "--repl did not persist the game")
endif()

run_cli(success add-game --name B --path "${TEST_DIR}/first game.rom")
file(READ "${TEST_DIR}/config/games.json" before)
run_cli(failure replace-game --old_name A --name B --path "${TEST_DIR}/second game.rom")
file(READ "${TEST_DIR}/config/games.json" after)
if(NOT before STREQUAL after)
	message(FATAL_ERROR "Failed replacement changed saved games")
endif()

run_cli(success replace-game --old_name A --name C --path "${TEST_DIR}/second game.rom")
run_cli(success list-game)
if(NOT CLI_OUTPUT MATCHES "C")
	message(FATAL_ERROR "Replacement missing from CLI list")
endif()

run_cli(success add-emu --name E --path "${TEST_DIR}/first game.rom" --launch-strategy-id direct --arg extra)
run_cli(success add-emu --name E --path "${TEST_DIR}/second game.rom" --launch-strategy-id direct --repl)
file(READ "${TEST_DIR}/config/emulators.json" emulators)
string(JSON saved_path GET "${emulators}" 0 path)
if(NOT saved_path STREQUAL "${TEST_DIR}/second game.rom")
	message(FATAL_ERROR "--repl did not persist the emulator")
endif()
run_cli(success replace-emu --old_name E --name E2 --path "${TEST_DIR}/second game.rom" --launch-strategy-id direct)
run_cli(success list-emu)
run_cli(success list-history)
if(NOT CLI_OUTPUT MATCHES "list-history")
	message(FATAL_ERROR "History should include the current command")
endif()
