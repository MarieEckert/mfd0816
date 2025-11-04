cmake_minimum_required(VERSION 3.31.0.)
project(mfd0816)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -DPRINT_FETCHED_INSTRUCTION")
set(CMAKE_CXX_FLAGS_RELEASE "-DRELEASE -O3")
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

set(MFD_INCLUDES ${PROJECT_SOURCE_DIR}
                 ${PROJECT_SOURCE_DIR}/3rdparty/doctest
                 ${PROJECT_SOURCE_DIR}/3rdparty/Arg3P
)

add_subdirectory(shared)
add_subdirectory(asm)
add_subdirectory(emu)

enable_testing()
add_subdirectory(test)