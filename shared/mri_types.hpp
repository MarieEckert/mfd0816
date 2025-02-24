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

#include "typedefs.hpp"

namespace shared::mri_types {

#pragma pack(push, 1)

#define MRI_MAGIC "MRI"
#define MRI_VERSION BIGENDIAN16(0x0100)

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

} // namespace shared::mri_types

#endif