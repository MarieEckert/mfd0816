cmake_minimum_required(VERSION 3.31.0.)
project(mfd0816)

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "-fno-exceptions -std=c++20 -stdlib=libc++ -Wall -I${PROJECT_SOURCE_DIR}/")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -DPRINT_FETCHED_INSTRUCTION")
set(CMAKE_CXX_FLAGS_RELEASE "-DRELEASE -O3")
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

add_subdirectory(shared)
add_subdirectory(asm)
add_subdirectory(emu)

enable_testing()
add_subdirectory(test)