add_executable(emu-test main.cpp
						arithmetic.cpp
						gio.cpp
						interrupt.cpp
)
target_link_libraries(emu-test PRIVATE emu shared)

add_test(NAME emu-test COMMAND emu-test --ni)
