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

#include <algorithm>
#include <unordered_map>
#include <vector>

#include <mfdasm/impl/directive_operand.hpp>

namespace mfdasm::impl {

/** @brief this is hell */
const std::unordered_map<Directive::Kind, std::vector<DirectiveOperand>>
	DirectiveOperand::m_directiveOperandsMap = {
		{
			Directive::DB,
			{
				DirectiveOperand({DirectiveOperand::IMMEDIATE}),
			},
		},
		{
			Directive::DW,
			{
				DirectiveOperand({DirectiveOperand::IMMEDIATE}),
			},
		},
		{
			Directive::DD,
			{
				DirectiveOperand({DirectiveOperand::IMMEDIATE}),
			},
		},
		{
			Directive::TIMES,
			{
				DirectiveOperand({DirectiveOperand::IMMEDIATE}),
			},
		},
		{
			Directive::DEFINE,
			{
				DirectiveOperand({DirectiveOperand::IMMEDIATE}),
				DirectiveOperand({DirectiveOperand::IMMEDIATE}),
			},
		},
};

DirectiveOperand::DirectiveOperand(std::vector<DirectiveOperand::Kind> allowed_kinds)
	: m_allowedKinds(allowed_kinds) {}

bool DirectiveOperand::isAllowed(Kind kind) const {
	return std::find(m_allowedKinds.cbegin(), m_allowedKinds.cend(), kind) != m_allowedKinds.cend();
}

std::vector<DirectiveOperand> DirectiveOperand::operandsFor(Directive::Kind directive) {
	const auto res = m_directiveOperandsMap.find(directive);

	return res != m_directiveOperandsMap.cend() ? res->second : std::vector<DirectiveOperand>{};
}
}  // namespace mfdasm::impl
