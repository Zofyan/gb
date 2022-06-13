cmake_minimum_required(VERSION 3.21)
project(gb)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -fsanitize=address")
set(GLOB SOURCE_FILES source/*.cpp)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(gb source/main.cpp include/loader.h source/instructions.cpp
        include/instructions.h source/cpu.cpp include/cpu.h
        source/bus.cpp include/bus.h include/logger.h source/logger.cpp
        source/ppu.cpp include/ppu.h source/lcd.cpp include/lcd.h
        source/fetcher.cpp include/fetcher.h)

target_link_libraries(gb ${SDL2_LIBRARIES})