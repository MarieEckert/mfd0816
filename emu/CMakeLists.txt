set(SOURCES
	mfdemu/impl/bus/aio_device.cpp
	mfdemu/impl/bus/gio_device.cpp
	mfdemu/impl/cpu.cpp
	mfdemu/impl/system.cpp
	mfdemu/mri.cpp
)

add_library(emu ${SOURCES})
target_include_directories(emu PRIVATE ${MFD_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(mfdemu mfdemu/main.cpp)
target_link_libraries(mfdemu emu shared)
target_include_directories(mfdemu PRIVATE ${MFD_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR})
