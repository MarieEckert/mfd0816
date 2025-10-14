set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${CMAKE_CURRENT_SOURCE_DIR} -I${PROJECT_SOURCE_DIR}/3rdparty/doctest -I${PROJECT_SOURCE_DIR}/asm -I${PROJECT_SOURCE_DIR}/emu")

set(CTEST_OUTPUT_ON_FAILURE TRUE)

add_subdirectory(mfdasm)
add_subdirectory(mfdemu)