add_executable(asm-test main.cpp
						encode.cpp
						parse.cpp
						parse_helper.cpp
)
target_link_libraries(asm-test PRIVATE asm shared)

add_test(NAME asm-test COMMAND asm-test --ni)