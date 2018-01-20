cmake_minimum_required (VERSION 2.6)
project (blackhole)
set(CMAKE_C_FLAGS "-g -Wall -O2")

set(SOURCES
    IO.c
    IO.h
    main.c
    MCTS.c
    MCTS.h
    Position.c
    Position.h
    Random.c
    Random.h
    Timer.c
    Timer.h
    TreeNode.c
    TreeNode.h)

add_executable(player ${SOURCES})

target_link_libraries(player m)
