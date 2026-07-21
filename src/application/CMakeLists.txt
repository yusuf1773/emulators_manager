add_library(application application.cpp)

target_link_libraries(application PRIVATE 
    CLI11::CLI11
    nlohmann_json::nlohmann_json
    comand_manager
)