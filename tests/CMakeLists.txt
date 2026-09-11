add_executable(launch_game_service_test unit/launch_game_service_test.cc)
target_include_directories(launch_game_service_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(launch_game_service_test PRIVATE emu_application emu_launch)
add_test(NAME launch_game_service COMMAND launch_game_service_test)

if(EMU_BUILD_PERSISTENCE)
  add_executable(persistence_test integration/persistence_test.cc)
  target_include_directories(persistence_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
  target_link_libraries(persistence_test PRIVATE emu_persistence)
  add_test(NAME persistence COMMAND persistence_test)
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
  domain/game
  domain/emulator
  domain/command_history_entry
  application/repository
  application/history_repository
  application/process_runner
  application/launch_context
  application/launch_strategy_info
  application/launch_strategy
  application/launch_strategy_registry
  application/launch_game_service
)

foreach(header IN LISTS core_headers)
  string(REPLACE "/" "_" name "${header}")
  set(source "${CMAKE_CURRENT_BINARY_DIR}/${name}_header.cc")
  file(WRITE "${source}" "#include \"emu/${header}.h\"\n")
  add_library("${name}_header" OBJECT "${source}")
  target_link_libraries("${name}_header" PRIVATE emu_domain)
endforeach()
