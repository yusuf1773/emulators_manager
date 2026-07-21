add_library(app_context app_context.cpp)

target_link_libraries(app_context PRIVATE 
    config
)