cmake_minimum_required(VERSION 3.28.0)

project(flappy-bird)

add_executable(flap)

target_sources(flap
    PRIVATE
    main.cpp
)

target_link_libraries(flap
    PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
)
