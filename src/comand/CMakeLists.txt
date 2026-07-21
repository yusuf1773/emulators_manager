add_library(comands add_game_command.cpp)

target_link_libraries(comands PRIVATE 
    CLI11::CLI11
    nlohmann_json::nlohmann_json
)