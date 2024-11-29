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

/**
 * @file ast.hpp
 * @brief Type definitions for the Abstract Syntax Tree used by mfdasm.
 *
 * # AST definition
 * @note Any part of a statement or expression which is surrounded by parenthesis, e.g.
 * ```
 *      (|> token(at))
 * ```
 * is not represented in the final AST but required within the input for that statement
 * or expression.
 *
 * ```
 * statement (section, Statement::SECTION)
 *     |> expression <identifier>
 *    (|> token (at))
 *     |> expression <number literal>
 *
 * statement (ld, Statement::INSTRUCTION{Instruction::LD})
 *     |> Expression::REGISTER
 *    (|> token (,))
 *     |> expression
 *
 * expression (literal, Expression::LITERAL)
 *     |> <literal>
 *
 * expression (identifier, Expression::IDENTIFIER)
 *     |> <identifier name>
 *
 * expression (direct addressing, EXPRESSION::DIRECT_ADDRESS)
 *    (|> token ([))
 *     |> expression (literal OR identifer)
 *    (|> token (]))
 *
 * expression (indirect addressing, EXPRESSION::INDIRECT_ADDRESS)
 *    (|> token ([))
 *    (|> token ([))
 *     |> expression (literal OR identifer)
 *    (|> token (]))
 *    (|> token (]))
 * ```
 */

#ifndef MFDASM_IMPL_AST_HPP
#define MFDASM_IMPL_AST_HPP

#include <string>
#include <vector>

#include <mfdasm/typedefs.hpp>

namespace mfdasm::impl {

class Instruction {
	enum Kind {
		ADC = 0x00,
		ADD = 0x01,
		AND = 0x02,
		BIN = 0x03,
		BOT = 0x04,
		CALL = 0x05,
		_RESERVED_00 = 0x06,
		CMP = 0x07,
		DEC = 0x08,
		DIV = 0x09,
		IDIV = 0x0a,
		IMUL = 0x0b,
		IN = 0x0c,
		INC = 0x0d,
		INT = 0x0e,
		IRET = 0x0f,
		JMP = 0x10,
		JZ = 0x11,
		JG = 0x12,
		JGE = 0x13,
		JL = 0x14,
		JLE = 0x15,
		JC = 0x16,
		JS = 0x17,
		JNZ = 0x18,
		JNC = 0x19,
		JNS = 0x1a,
		LD = 0x1b,
		MOV = 0x1c,
		MUL = 0x1d,
		NEG = 0x1e,
		NOP = 0x1f,
		NOT = 0x20,
		OR = 0x21,
		OUT = 0x22,
		POP = 0x23,
		PUSH = 0x24,
		RET = 0x25,
		ROL = 0x26,
		ROR = 0x27,
		SL = 0x28,
		SR = 0x29,
		ST = 0x2a,
		CLO = 0x2b,
		CLC = 0x2c,
		CLZ = 0x2d,
		CLN = 0x2e,
		CLI = 0x2f,
		_RESERVED_01 = 0x30,
		_RESERVED_02 = 0x31,
		_RESERVED_03 = 0x32,
		_RESERVED_04 = 0x33,
		_RESERVED_05 = 0x34,
		_RESERVED_06 = 0x35,
		_RESERVED_07 = 0x36,
		_RESERVED_08 = 0x37,
		_RESERVED_09 = 0x38,
		_RESERVED_10 = 0x39,
		_RESERVED_11 = 0x3a,
		STO = 0x3b,
		STC = 0x3c,
		STZ = 0x3d,
		STN = 0x3e,
		STI = 0x3f,
		_RESERVED_12 = 0x40,
		_RESERVED_13 = 0x41,
		_RESERVED_14 = 0x42,
		_RESERVED_15 = 0x43,
		_RESERVED_16 = 0x44,
		_RESERVED_17 = 0x45,
		_RESERVED_18 = 0x46,
		_RESERVED_19 = 0x47,
		_RESERVED_20 = 0x48,
		_RESERVED_21 = 0x49,
		_RESERVED_22 = 0x4a,
		SUB = 0x4b,
		TEST = 0x4c,
		XOR = 0x4d,
	};
};

class Directive {
	enum Kind {
		/* directives */
	};
};

class ExpressionBase {};

class Literal : public ExpressionBase {
	enum Kind {
		HEXADECIMAL_LITERAL,
		DECIMAL_LITERAL,
		BINARY_LITERAL,
		STRING_LITERAL,
	};

	Literal(Kind kind, std::vector<u8> value);
};

class Identifier : public ExpressionBase {
	enum Kind {
		LABEL,
		SECTION,
	};

	Identifier(Kind kind, std::string name, std::vector<u8> value);
};

class Expression : public ExpressionBase {
	enum Kind {
		LITERAL,
		IDENTIFIER,
		DIRECT_ADDRESS,
		INDIRECT_ADDRESS,
	};

	Expression(Kind kind, std::vector<ExpressionBase> expressions);
};

class Statement {
	enum Kind {
		SECTION,
		ADDRESSING_ABSOLUTE,
		ADDRESSING_RELATIVE,
		INSTRUCTION,
		DIRECTIVE,
	};

	Statement(Kind kind, std::vector<Expression> expressions);
};
}  // namespace mfdasm::impl

#endif