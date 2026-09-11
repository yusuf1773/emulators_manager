include(FetchContent)

if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

if(EMU_BUILD_PERSISTENCE)
  find_package(nlohmann_json 3.12.0 EXACT CONFIG QUIET)
  if(NOT TARGET nlohmann_json::nlohmann_json)
    FetchContent_Declare(json
      URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
      URL_HASH SHA256=42f6e95cad6ec532fd372391373363b62a14af6d771056dbfc86160e6dfff7aa
    )
    FetchContent_MakeAvailable(json)
  endif()
endif()

if(EMU_BUILD_CLI)
  find_package(CLI11 2.5.0 EXACT CONFIG QUIET)
  if(NOT TARGET CLI11::CLI11)
    FetchContent_Declare(CLI11
      GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
      GIT_TAG 4160d259d961cd393fd8d67590a8c7d210207348
    )
    FetchContent_MakeAvailable(CLI11)
  endif()
endif()
