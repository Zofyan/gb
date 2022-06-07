cmake_minimum_required(VERSION 3.21)
project(gb)

set(CMAKE_CXX_STANDARD 14)

set(GLOB SOURCE_FILES source/*.cpp)

find_package(SDL2 REQUIRED SDL2)

add_executable(gb source/main.cpp include/loader.h source/instructions.cpp
        include/instructions.h source/cpu.cpp include/cpu.h
        source/bus.cpp include/bus.h include/ppu.h source/ppu.cpp include/oam.h
        source/oam.cpp include/logger.h source/logger.cpp
        source/fetcher.cpp include/fetcher.h source/lcd.cpp include/lcd.h)

target_link_libraries(gb
        PRIVATE SDL2::SDL2
        )