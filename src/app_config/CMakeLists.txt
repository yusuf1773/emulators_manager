add_library(config config.cpp)

target_link_libraries(config PUBLIC
    nlohmann_json::nlohmann_json
)

target_include_directories(config PUBLIC ${CMAKE_SOURCE_DIR})