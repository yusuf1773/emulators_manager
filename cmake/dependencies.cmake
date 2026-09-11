include(FetchContent)

if(POLICY CMP0135)
	cmake_policy(SET CMP0135 NEW)
endif()

if(EMU_BUILD_PERSISTENCE)
	find_package(SQLite3 QUIET)
	if(NOT TARGET SQLite::SQLite3)
		FetchContent_Declare(sqlite
			URL https://www.sqlite.org/2026/sqlite-amalgamation-3530400.zip
			URL_HASH SHA3_256=628a44cfe82c66aed1ccbbe85a562d2e33ebe64b3288981ed76285612227934e
		)
		FetchContent_MakeAvailable(sqlite)
		add_library(sqlite3_vendor STATIC "${sqlite_SOURCE_DIR}/sqlite3.c")
		target_include_directories(sqlite3_vendor PUBLIC "${sqlite_SOURCE_DIR}")
		add_library(SQLite::SQLite3 ALIAS sqlite3_vendor)
	endif()

	find_package(nlohmann_json 3.12.0 EXACT CONFIG QUIET)
	if(NOT TARGET nlohmann_json::nlohmann_json)
		FetchContent_Declare(json
			URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
			URL_HASH SHA256=42f6e95cad6ec532fd372391373363b62a14af6d771056dbfc86160e6dfff7aa
		)
		FetchContent_MakeAvailable(json)
	endif()
endif()

if(BUILD_TESTING)
	find_package(GTest 1.17.0 CONFIG QUIET)
	if(NOT TARGET GTest::gtest_main)
		set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
		FetchContent_Declare(googletest
			GIT_REPOSITORY https://github.com/google/googletest.git
			GIT_TAG v1.17.0
			GIT_SHALLOW TRUE
		)
		FetchContent_MakeAvailable(googletest)
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

if(EMU_BUILD_GUI)
	set(emu_qt_components Core Gui Qml Quick QuickControls2)
	if(BUILD_TESTING)
		list(APPEND emu_qt_components Test)
	endif()
	find_package(Qt6 6.5 REQUIRED COMPONENTS ${emu_qt_components})
endif()
