file(GLOB UNIT_TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/unit/*.cpp")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${CMAKE_CURRENT_SOURCE_DIR} -I${PROJECT_SOURCE_DIR}/3rdparty/doctest -I${PROJECT_SOURCE_DIR}/asm -I${PROJECT_SOURCE_DIR}/emu")

set(CTEST_OUTPUT_ON_FAILURE TRUE)

foreach(TEST_SRC ${UNIT_TEST_SOURCES})
	get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)

	add_executable(${TEST_NAME} ${TEST_SRC})
	target_link_libraries(${TEST_NAME} PRIVATE asm emu shared)

	add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME} --ni)
endforeach()