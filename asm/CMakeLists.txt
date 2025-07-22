set(SOURCES
	mfdasm/impl/asmerror.cpp
	mfdasm/impl/assembler.cpp
	mfdasm/impl/ast.cpp
	mfdasm/impl/directive_operand.cpp
	mfdasm/impl/instruction_operand.cpp
	mfdasm/impl/mri/mri.cpp
	mfdasm/impl/mri/section_table.cpp
	mfdasm/impl/token.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${CMAKE_CURRENT_SOURCE_DIR}")

add_library(asm ${SOURCES})
add_executable(mfdasm mfdasm/main.cpp)
target_link_libraries(mfdasm asm shared)