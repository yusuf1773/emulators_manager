add_library(strategy
    direct_strategy.cpp
)

target_include_directories(strategy PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)