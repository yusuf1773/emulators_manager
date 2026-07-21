add_library(comand_manager comand_manager.cpp)

target_link_libraries(comand_manager PRIVATE 
    nlohmann_json::nlohmann_json
    CLI11::CLI11
    comands
    app_context
)