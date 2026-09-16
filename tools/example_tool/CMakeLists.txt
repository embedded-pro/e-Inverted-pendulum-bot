add_executable(inverted_pendulum_bot.tool.example)

target_include_directories(inverted_pendulum_bot.tool.example PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../../>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries(inverted_pendulum_bot.tool.example PUBLIC
    infra.util
    inverted_pendulum_bot.example.implementations
)

target_sources(inverted_pendulum_bot.tool.example PRIVATE
    Main.cpp
)
