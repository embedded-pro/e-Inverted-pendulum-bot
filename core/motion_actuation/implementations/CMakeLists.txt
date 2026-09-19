add_library(inverted_pendulum_bot.motion_actuation.implementations ${EMIL_EXCLUDE_FROM_ALL})

target_include_directories(inverted_pendulum_bot.motion_actuation.implementations PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../../../>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries(inverted_pendulum_bot.motion_actuation.implementations PUBLIC
    inverted_pendulum_bot.motion_actuation.interfaces
    inverted_pendulum_bot.platform_abstraction
)

target_sources(inverted_pendulum_bot.motion_actuation.implementations PRIVATE
    MotionActuationImpl.hpp
    MotionActuationImpl.cpp
)

add_subdirectory(test)
