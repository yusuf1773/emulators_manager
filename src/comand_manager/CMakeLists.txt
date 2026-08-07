add_library(comand_manager comand_manager.cpp)

target_link_libraries(comand_manager PRIVATE
    CLI11::CLI11
    comands
)