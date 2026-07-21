cmake_minimum_required(VERSION 3.20)

project(emu_manager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    CLI11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.5.0
    GIT_SHALLOW TRUE
)

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)

FetchContent_MakeAvailable(json)

FetchContent_MakeAvailable(CLI11)

add_subdirectory(src/launch_strategy)
add_subdirectory(src/app_config)
add_subdirectory(src/app_context)
add_subdirectory(src/comand)
add_subdirectory(src/comand_manager)
add_subdirectory(src/application)

add_executable(emu_manager
    src/main.cpp
)

target_link_libraries(emu_manager PUBLIC 
    CLI11::CLI11
    nlohmann_json::nlohmann_json
    strategy
    comands
    config
    app_context
    comand_manager
    application
)