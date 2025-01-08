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

Result<std::shared_ptr<Section>, AsmError> SectionTable::addFromStatement(const Statement &statement, ResolvalContext &resolval_context) {
	if(resolval_context.identifiers.contains("TODO")) {
		return Err(AsmError(AsmError::IDENTIFIER_REDEFINITION, statement.lineno()));
	}

	auto section = std::make_shared<Section>();
	section->offset = 0; /* todo */

	m_sectionMap.emplace("TODO", section);
	resolval_context.identifiers.emplace("TODO", section->offset);

	return Ok(section);
}

const SectionMap &SectionTable::sectionMap() const {
	return m_sectionMap;
}

} // namespace mfdasm::impl::mri
