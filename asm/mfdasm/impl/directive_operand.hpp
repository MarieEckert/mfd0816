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

#ifndef MFDASM_IMPL_DIRECTIVE_OPERAND_HPP
#define MFDASM_IMPL_DIRECTIVE_OPERAND_HPP

#include <vector>

#include <mfdasm/impl/ast.hpp>

namespace mfdasm::impl {

class DirectiveOperand {
   public:
	enum Kind {
		IMMEDIATE = 0,
		UNKNOWN = 1,
		INVALID = 255,
	};

	static std::vector<DirectiveOperand> operandsFor(Directive::Kind directive);

	bool isAllowed(Kind kind) const;

   private:
	DirectiveOperand(std::vector<Kind> allowed_kinds);

	static const std::unordered_map<Directive::Kind, std::vector<DirectiveOperand>>
		m_directiveOperandsMap;

	std::vector<Kind> m_allowedKinds;
};

}  // namespace mfdasm::impl

#endif
