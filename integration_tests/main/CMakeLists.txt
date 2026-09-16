add_executable(inverted_pendulum_bot.integration_tests)
emil_build_for(inverted_pendulum_bot.integration_tests HOST All BOOL INVERTED_PENDULUM_BOT_BUILD_TESTS)

target_link_libraries(inverted_pendulum_bot.integration_tests PRIVATE
    inverted_pendulum_bot.integration_tests.steps
    inverted_pendulum_bot.integration_tests.hooks
    inverted_pendulum_bot.integration_tests.support
    cucumber_cpp.runner
)

add_test(NAME inverted_pendulum_bot.integration_tests
    COMMAND inverted_pendulum_bot.integration_tests -- ${CMAKE_CURRENT_SOURCE_DIR}/../features)
set_tests_properties(inverted_pendulum_bot.integration_tests PROPERTIES LABELS integration)
