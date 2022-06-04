cmake_minimum_required(VERSION 3.22)
project(gb)

set(CMAKE_CXX_STANDARD 14)

set(GLOB SOURCE_FILES source/*.cpp)

add_executable(gb source/main.cpp source/loader.cpp include/loader.h source/instructions.cpp include/instructions.h source/memory.cpp include/memory.h source/cpu.cpp include/cpu.h source/bus.cpp include/bus.h source/logger.cpp include/logger.h)
