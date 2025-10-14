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

#ifndef SHARED_MRI_TYPES_HPP
#define SHARED_MRI_TYPES_HPP

#include "int_ops.hpp"
#include "typedefs.hpp"

namespace shared::mri_types {

#pragma pack(push, 1)

#define MRI_MAGIC "MRI"
#define MRI_VERSION BIGENDIAN16(0x0100)

constexpr usize HEADER_MAGIC_OFFSET = 0;
constexpr usize HEADER_VERSION_OFFSET = 4;
constexpr usize HEADER_TYPE_OFFSET = 6;
constexpr usize HEADER_FILESIZE_OFFSET = 8;
constexpr usize HEADER_DATA_OFFSET_OFFSET = 12;
constexpr usize TABLE_ENTRY_COUNT_OFFSET = 16;
constexpr usize TABLE_ENTRY_START_OFFSET = 20;

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

#pragma pack(pop)

constexpr usize MRI_MIN_SIZE = sizeof(shared::mri_types::Header) + 1;

}  // namespace shared::mri_types

#endif