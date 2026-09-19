add_library(inverted_pendulum_bot.wheel_odometry.interfaces INTERFACE ${EMIL_EXCLUDE_FROM_ALL})

target_include_directories(inverted_pendulum_bot.wheel_odometry.interfaces INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../../../>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_sources(inverted_pendulum_bot.wheel_odometry.interfaces PRIVATE
    WheelOdometry.hpp
)
