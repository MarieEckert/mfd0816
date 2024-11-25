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
	sasm::impl::Assembler asem;
	asem.parseLines(DEBUG_ASM_SOURCE);
	return 0;
}