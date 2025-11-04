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

add_library(asm ${SOURCES})
target_include_directories(asm PRIVATE ${MFD_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(mfdasm mfdasm/main.cpp)
target_link_libraries(mfdasm asm shared)
target_include_directories(mfdasm PUBLIC ${MFD_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR})
