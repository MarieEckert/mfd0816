set(SOURCES
	cli/args.cpp
	log.cpp
	panic.cpp
)

add_library(shared ${SOURCES})