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

#ifndef MFDASM_IMPL_MRI_SECTION_TABLE_HPP
#define MFDASM_IMPL_MRI_SECTION_TABLE_HPP

#include <map>
#include <memory>
#include <string>

#include <mfdasm/impl/asmerror.hpp>
#include <mfdasm/impl/ast.hpp>
#include <mfdasm/typedefs.hpp>

namespace mfdasm::impl::mri {

struct Section {
	u16 offset;
	std::vector<u8> data;
};

using SectionMap = std::map<std::string, std::shared_ptr<Section>>;

class SectionTable {
   public:
	Result<std::shared_ptr<Section>, AsmError> addFromStatement(
		const Statement &statement,
		ResolvalContext &resolval_context);

	const SectionMap &sectionMap() const;

   private:
	SectionMap m_sectionMap;
};

}  // namespace mfdasm::impl::mri

#endif
