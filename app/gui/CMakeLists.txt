qt_standard_project_setup(REQUIRES 6.5)

if(Qt6_VERSION VERSION_GREATER_EQUAL 6.8)
	qt_policy(SET QTP0004 NEW)
endif()

add_library(emu_gui_adapter STATIC
	error_notifier.cpp
	error_notifier.h
	gui_app_context.cpp
	gui_app_context.h
	models/emulator_list_model.cpp
	models/emulator_list_model.h
	models/game_list_model.cpp
	models/game_list_model.h
)
target_include_directories(emu_gui_adapter PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(emu_gui_adapter PUBLIC
	emu_application
	Qt6::Core
)
target_link_libraries(emu_gui_adapter PRIVATE
	emu_persistence
)

qt_add_executable(emu_manager_gui
	main.cpp
)

qt_add_qml_module(emu_manager_gui
	URI EmuManager
	VERSION 1.0
	QML_FILES
		qml/Main.qml
)

target_link_libraries(emu_manager_gui PRIVATE
	emu_gui_adapter
	Qt6::Core
	Qt6::Gui
	Qt6::Qml
	Qt6::Quick
	Qt6::QuickControls2
)

set_target_properties(emu_manager_gui PROPERTIES
	MACOSX_BUNDLE TRUE
	RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
	WIN32_EXECUTABLE TRUE
)

if(BUILD_TESTING)
	add_executable(game_list_model_qt_test
		"${PROJECT_SOURCE_DIR}/tests/gui/game_list_model_test.cpp"
	)
	target_include_directories(game_list_model_qt_test PRIVATE
		"${PROJECT_SOURCE_DIR}/tests"
	)
	target_link_libraries(game_list_model_qt_test PRIVATE
		emu_gui_adapter
		Qt6::Test
	)
	add_test(NAME gui_game_list_model COMMAND game_list_model_qt_test)

	add_test(NAME gui_smoke
		COMMAND emu_manager_gui --smoke-test
	)
	set_tests_properties(gui_smoke PROPERTIES
		ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_QUICK_BACKEND=software"
		TIMEOUT 10
	)
endif()
