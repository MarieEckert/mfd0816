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

#include <mfdasm/impl/mri/section_table.hpp>

namespace mfdasm::impl::mri {

/* class SectionTable */

Result<std::shared_ptr<Section>, AsmError> SectionTable::addFromStatement(
	const Statement &statement,
	ResolvalContext &resolval_context) {
	const std::vector<std::shared_ptr<ExpressionBase>> &expressions = statement.expressions();
	/** @todo I think i may be starting to go overboard with these safety checks */
	if(statement.kind() != Statement::SECTION || expressions.size() != 2 ||
	   expressions[0]->kind() != ExpressionBase::IDENTIFIER ||
	   expressions[1]->kind() != ExpressionBase::LITERAL) {
		return Err(AsmError(
			AsmError::SYNTAX_ERROR, statement.lineno(), "section missing name or address"));
	}

	const std::shared_ptr<Identifier> identifier =
		std::static_pointer_cast<Identifier>(expressions[0]);
	const std::shared_ptr<Literal> literal = std::static_pointer_cast<Literal>(expressions[1]);

	if(resolval_context.identifiers.contains(identifier->name())) {
		return Err(AsmError(AsmError::IDENTIFIER_REDEFINITION, statement.lineno()));
	}

	const Result<std::vector<u8>, AsmError> maybe_literal_value =
		literal->resolveValue(resolval_context);
	if(maybe_literal_value.isErr()) {
		return Err(maybe_literal_value.unwrapErr());
	}

	const std::vector<u8> literal_value = maybe_literal_value.unwrap();

	if(literal_value.size() != 2) {
		panic("section position literal constructed with invalid byte size!");
	}

	auto section = std::make_shared<Section>();
	section->offset = (literal_value[0] << 8) | literal_value[1];

	m_sectionMap.emplace(identifier->name(), section);
	resolval_context.identifiers.emplace(identifier->name(), section->offset);

	return Ok(section);
}

const SectionMap &SectionTable::sectionMap() const {
	return m_sectionMap;
}

}  // namespace mfdasm::impl::mri
