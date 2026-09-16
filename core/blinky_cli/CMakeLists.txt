add_library(inverted_pendulum_bot.blinky_cli ${EMIL_EXCLUDE_FROM_ALL})

target_include_directories(inverted_pendulum_bot.blinky_cli PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../../>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries(inverted_pendulum_bot.blinky_cli PUBLIC
    inverted_pendulum_bot.platform_abstraction
    services.util
    services.peripheral
    services.tracer
    infra.timer
    infra.util
)

target_sources(inverted_pendulum_bot.blinky_cli PRIVATE
    BlinkyCli.hpp
    BlinkyCli.cpp
)

add_subdirectory(test)
