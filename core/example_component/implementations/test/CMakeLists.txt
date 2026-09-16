add_executable(inverted_pendulum_bot.example_test)
emil_build_for(inverted_pendulum_bot.example_test HOST All BOOL INVERTED_PENDULUM_BOT_BUILD_TESTS)
emil_add_test(inverted_pendulum_bot.example_test)

target_link_libraries(inverted_pendulum_bot.example_test PUBLIC
    gmock_main
    inverted_pendulum_bot.example.implementations
)

target_sources(inverted_pendulum_bot.example_test PRIVATE
    TestAccumulatorImpl.cpp
)
