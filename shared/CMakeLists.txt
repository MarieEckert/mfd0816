set(SOURCES
	cli/args.cpp
	log.cpp
	panic.cpp
)

add_library(shared ${SOURCES})
target_include_directories(shared PRIVATE ${MFD_INCLUDES})