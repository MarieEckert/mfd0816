set(SOURCES
	mfdemu/impl/bus/aio_device.cpp
	mfdemu/impl/bus/gio_device.cpp
	mfdemu/impl/cpu.cpp
	mfdemu/impl/system.cpp
	mfdemu/mri.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${CMAKE_CURRENT_SOURCE_DIR}")

add_library(emu ${SOURCES})
add_executable(mfdemu mfdemu/main.cpp)
target_link_libraries(mfdemu emu shared)