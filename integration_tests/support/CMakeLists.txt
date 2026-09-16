add_library(inverted_pendulum_bot.integration_tests.support OBJECT
    AccumulatorFixture.cpp
    AccumulatorFixture.hpp
)

target_include_directories(inverted_pendulum_bot.integration_tests.support PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../../>"
)

target_link_libraries(inverted_pendulum_bot.integration_tests.support PUBLIC
    inverted_pendulum_bot.example.implementations
    GTest::gtest
    GTest::gmock
)
