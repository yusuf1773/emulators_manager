add_library(comands
        add_game_command.cpp
        list_games_command.cpp
        add_emu_command.cpp
        list_launch_strategies_command.cpp
        list_emus_command.cpp
        launch_game_command.cpp
        replace_emu_command.cpp
        replace_game_command.cpp
        list_command_history_command.cpp
        icommand.cpp
)

target_link_libraries(comands PRIVATE 
    CLI11::CLI11
    nlohmann_json::nlohmann_json
)