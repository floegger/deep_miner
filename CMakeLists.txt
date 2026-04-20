cmake_minimum_required(VERSION 3.15)
project(deep_miner)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(SOURCES
    src/BaseRobot.cpp
    src/SortBot.cpp
    src/World.cpp
    src/DigDeepBot.cpp
    src/Game.cpp
    src/RandomBot.cpp
)

add_executable(deep_miner
    ${SOURCES}
    main.cpp
)

add_executable(deep_miner_tests
    ${SOURCES}
    tests/test_all.cpp
)
