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

#include <mfdasm/typedefs.hpp>
#include <mfdasm/impl/mri/section_table.hpp>

namespace mfdasm::impl::mri {

#pragma pack(push, 4)

constexpr char mri_magic[4] = "MRI";
constexpr u16 mri_version = 0x0100;

enum class TypeFlag : u16 {
	COMPACT = 1 << 0,
	DATA_COMPRESSED = 1 << 1,
};

struct Header {
	char magic[4];
	u16 version;
	u16 type;
	u32 filesize;
	u32 data_offset;
};

struct TableEntry {
	u32 file_offset;
	u16 load_address;
	u16 length;
};

struct Table {
	u32 entry_count;
	TableEntry entries[];
};

#pragma pack(pop)

static void writeCompactMRI(std::string path, SectionTable sections, bool compressed);

static void writePaddedMRI(std::string path, SectionTable sections, bool compressed);

} // namespace mfdasm::impl::mri

#endif
