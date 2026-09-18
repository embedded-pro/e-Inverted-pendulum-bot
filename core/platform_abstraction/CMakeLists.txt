add_library(inverted_pendulum_bot.platform_abstraction INTERFACE ${EMIL_EXCLUDE_FROM_ALL})

target_include_directories(inverted_pendulum_bot.platform_abstraction INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../../>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries(inverted_pendulum_bot.platform_abstraction INTERFACE
    hal.interfaces
    hal.synchronous_interfaces
    services.tracer
)

target_sources(inverted_pendulum_bot.platform_abstraction PRIVATE
    MotorBridge.hpp
    Platform.hpp
)

if (INVERTED_PENDULUM_BOT_BUILD_TESTS)
    add_subdirectory(test_doubles)
endif()
