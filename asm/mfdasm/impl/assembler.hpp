/*
 * Copyright (C) 2024  Marie Eckert
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef MFDASM_IMPL_ASSEMBLER_HPP
#define MFDASM_IMPL_ASSEMBLER_HPP

#include <vector>

#include <mfdasm/impl/asmerror.hpp>
#include <mfdasm/impl/ast.hpp>
#include <mfdasm/impl/token.hpp>
#include <mfdasm/result.hpp>

namespace mfdasm::impl {

/**
 * @brief Main interface for assembling some source code into machine code.
 */
class Assembler {
   public:
	/**
	 * @brief Lexes and parses the given input into an AST.
	 * @param source The complete source code to be parsed.
	 */
	Result<None, AsmError> parseLines(const std::string &source);

   private:
	Result<None, AsmError> lexInput(const std::string &source);

	u32 parseStringLiteral(const std::string &source, u32 &lineno);

	Result<None, AsmError> parseTokens();

	Result<u32, AsmError> tryParseSectionAt(u32 ix);

	Result<u32, AsmError> tryParseLabelAt(u32 ix);

	Result<u32, AsmError> tryParseAddressingStatementAt(u32 ix);

	/**
	 * @brief Attempts to parse a singular instruction or directive statement
	 * from m_tokens tarting at index ix.
	 */
	Result<u32, AsmError> tryParseUnknownAt(u32 ix);

	Result<u32, AsmError> tryParseInstruction(u32 ix, Instruction::Kind kind);

	Result<u32, AsmError> tryParseDirective(u32 ix, Directive::Kind kind);

	Result<std::pair<u32, std::vector<ExpressionBase>>, AsmError> tryParseOperands(u32 ix);

	std::vector<Token> m_tokens;

	std::vector<Statement> m_ast;
};

}  // namespace mfdasm::impl

#endif