add_executable(inverted_pendulum_bot.wheel_odometry_test)
emil_build_for(inverted_pendulum_bot.wheel_odometry_test HOST All BOOL INVERTED_PENDULUM_BOT_BUILD_TESTS)
emil_add_test(inverted_pendulum_bot.wheel_odometry_test)

target_link_libraries(inverted_pendulum_bot.wheel_odometry_test PUBLIC
    gmock_main
    inverted_pendulum_bot.wheel_odometry.implementations
    inverted_pendulum_bot.platform_abstraction_mock
    hal.synchronous_interfaces_test_doubles
    infra.timer_test_helper
)

target_sources(inverted_pendulum_bot.wheel_odometry_test PRIVATE
    TestWheelOdometryImpl.cpp
)
