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
		/* instructions */
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