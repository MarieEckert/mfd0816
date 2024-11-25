#include <string>

#include <cli/args.hpp>
#include <impl/assembler.hpp>

static const std::string DEBUG_ASM_SOURCE =
	"section code at 0x1100\n"
	"	_entry:		mov 0x1000, sp	; initialise stack pointer\n"
	"			ld  acl, [sp]   ; load the word from the top of the\n"
	"					  stack into acl\n"
	"			ld  bcl, [[0x1330]]\n"
	"\n"
	"section data at 0xd000\n"
	"	buffer:		times 1024 db 0\n"
	"	buffer_size:	dw _here - buffer\n"
	"	text:		'hello, world!' db 0\n"
	"\n"
	"section reset_vector at 0xfffe\n"
	"	reset_vector:	dw code\n";

int main(void) {
	return 0;
}
