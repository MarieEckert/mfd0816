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

#ifndef MFDASM_IMPL_INSTRUCTION_OPERAND_HPP
#define MFDASM_IMPL_INSTRUCTION_OPERAND_HPP

#include <vector>

#include <mfdasm/impl/ast.hpp>

namespace mfdasm::impl {

class InstructionOperand {
   public:
	enum Kind { /* Relative variants are deliberately left out here */
				IMMEDIATE = 0b0000,
				DIRECT = 0b0001,
				INDIRECT = 0b0010,
				REGISTER_IMMEDIATE = 0b1000,
				REGISTER_DIRECT = 0b1001,
				REGISTER_INDIRECT = 0b1010,
	};

	static std::vector<InstructionOperand> operandsFor(Instruction::Kind instruction);

   private:
	InstructionOperand(std::vector<Kind> allowed_kinds);

	bool isAllowed(Kind kind) const;

	static const std::unordered_map<Instruction::Kind, std::vector<InstructionOperand>>
		m_instructionOperandsMap;

	std::vector<Kind> m_allowedKinds;
};

}  // namespace mfdasm::impl

#endif