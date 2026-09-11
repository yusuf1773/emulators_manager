qt_standard_project_setup(REQUIRES 6.5)

if(Qt6_VERSION VERSION_GREATER_EQUAL 6.8)
	qt_policy(SET QTP0004 NEW)
endif()

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
	add_test(NAME gui_smoke
		COMMAND emu_manager_gui --smoke-test
	)
	set_tests_properties(gui_smoke PROPERTIES
		ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_QUICK_BACKEND=software"
		TIMEOUT 10
	)
endif()
