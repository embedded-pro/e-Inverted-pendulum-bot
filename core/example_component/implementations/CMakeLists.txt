add_library(inverted_pendulum_bot.example.implementations ${EMIL_EXCLUDE_FROM_ALL})

target_include_directories(inverted_pendulum_bot.example.implementations PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../../../>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries(inverted_pendulum_bot.example.implementations PUBLIC
    inverted_pendulum_bot.example.interfaces
)

target_sources(inverted_pendulum_bot.example.implementations PRIVATE
    AccumulatorImpl.hpp
    AccumulatorImpl.cpp
)

add_subdirectory(test)
