add_executable(inverted_pendulum_bot.example_app)

target_include_directories(inverted_pendulum_bot.example_app PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../../>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries(inverted_pendulum_bot.example_app PUBLIC
    infra.util
    inverted_pendulum_bot.example.implementations
)

target_sources(inverted_pendulum_bot.example_app PRIVATE
    Main.cpp
)
