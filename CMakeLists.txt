cmake_minimum_required(VERSION 4.2)
project(deep_miner)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_executable(deep_miner
        include/BaseRobot.h
        include/Robot.h
        include/SortBot.h
        include/World.h
        src/BaseRobot.cpp
        src/SortBot.cpp
        src/World.cpp
        include/Game.h
        include/DigDeepBot.h
        include/RandomBot.h
        src/DigDeepBot.cpp
        src/Game.cpp
        src/RandomBot.cpp
        main.cpp)
