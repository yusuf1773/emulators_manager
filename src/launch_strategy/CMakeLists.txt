add_library(strategy
    direct_strategy.cpp
    launch_strategy_manager.cpp
)

target_include_directories(strategy PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(strategy
    PUBLIC
        nlohmann_json::nlohmann_json
)