add_library(inverted_pendulum_bot.integration_tests.hooks OBJECT
    Hooks.cpp
)

target_link_libraries(inverted_pendulum_bot.integration_tests.hooks PUBLIC
    cucumber_cpp.runner
)
