/* This file is part of MFDASM.
 *
 * MFDASM is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * MFDASM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * MFDASM. If not, see <https://www.gnu.org/licenses/>.
 */

#include <fstream>
#include <sstream>
#include <string>

#include <mfdasm/cli/args.hpp>
#include <mfdasm/impl/assembler.hpp>
#include <mfdasm/impl/ast.hpp>
#include <mfdasm/log.hpp>

static const std::string DEBUG_ASM_SOURCE =
	"section code at 0x1100\n"
	"	_entry:		mov 0x1000, sp	; initialise stack pointer\n"
	"			ld  acl, [sp]   ; load the word from the top of the\n"
	"					        ; stack into acl\n"
	"			ld  bcl, [[0x1330]]\n"
	"\n"
	"section data at 0xd000\n"
	"	buffer:		times 1024 db 0\n"
	"	buffer_size:	dw _here - buffer\n"
	"	text:		\"hello, world! \\\\ \\\" \\\\\\\"\" db 0\n"
	"\n"
	"section reset_vector at 0xfffe\n"
	"	reset_vector:	dw code\n";

int main(void) {
	std::stringstream buffer;
	std::ifstream instream("test.asm");

	buffer << instream.rdbuf();

	mfdasm::impl::Assembler asem;
	Result<None, mfdasm::impl::AsmError> asm_res = asem.parseLines(buffer.str());

	if(asm_res.isErr()) {
		logError() << "An error occured whilst assembling: " << asm_res.unwrapErr().toString()
				   << "\n";
		std::exit(100);
	}

	mfdasm::impl::Instruction test_instruction(mfdasm::impl::Instruction::_RESERVED_00, {});
	mfdasm::impl::Statement test(mfdasm::impl::Statement::INSTRUCTION, {});
	return 0;
}