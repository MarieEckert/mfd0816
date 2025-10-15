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

#ifndef MFDASM_IMPL_MRI_MRI_HPP
#define MFDASM_IMPL_MRI_MRI_HPP

#include <shared/int_ops.hpp>
#include <shared/typedefs.hpp>

#include <mfdasm/impl/mri/section_table.hpp>

namespace mfdasm::impl::mri {

void writeCompactMRI(const std::string &path, const SectionTable &sections, bool compressed);

void writePaddedMRI(const std::string &path, const SectionTable &sections, bool compressed);

}  // namespace mfdasm::impl::mri

#endif
