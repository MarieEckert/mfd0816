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

#ifndef MFDASM_INT_OPS_HPP
#define MFDASM_INT_OPS_HPP

#include <vector>

#include <endian.h>

#include <mfdasm/typedefs.hpp>

namespace mfdasm::intops {

[[nodiscard]] static inline std::vector<u8> u64ToBytes(u64 value) {
	return {
		static_cast<u8>((value >> 56) & 0xFF), static_cast<u8>((value >> 48) & 0xFF),
		static_cast<u8>((value >> 40) & 0xFF), static_cast<u8>((value >> 32) & 0xFF),
		static_cast<u8>((value >> 24) & 0xFF), static_cast<u8>((value >> 16) & 0xFF),
		static_cast<u8>((value >> 8) & 0xFF),  static_cast<u8>((value >> 0) & 0xFF),
	};
}

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define BIGENDIAN16(x) intops::swapendian16(x)
#define BIGENDIAN32(x) intops::swapendian32(x)
#define BIGENDIAN64(x) intops::swapendian64(x)
#define LITTLEENDIAN16(x) x
#define LITTLEENDIAN32(x) x
#define LITTLEENDIAN64(x) x
#elif __BYTE_ORDER__ == __BIG_ENDIAN
#define BIGENDIAN16(x) x
#define BIGENDIAN32(x) x
#define BIGENDIAN64(x) x
#define LITTLEENDIAN16(x) intops::swapendian16(x)
#define LITTLEENDIAN32(x) intops::swapendian32(x)
#define LITTLEENDIAN64(x) intops::swapendian64(x)
#endif

[[nodiscard]] static inline u16 swapendian16(u16 value) {
	return ((value << 8) & 0xFF00) | ((value >> 8) & 0x00FF);
}

[[nodiscard]] static inline u32 swapendian32(u32 value) {
	return ((value & 0x000000FF) << 24) | ((value & 0x0000FF00) << 8) |
		   ((value & 0x00FF0000) >> 8) | ((value & 0xFF000000) >> 24);
}

[[nodiscard]] static inline u64 swapendian64(u64 value) {
	return ((value & 0x00000000000000FF) << 56) | ((value & 0x000000000000FF00) << 40) |
		   ((value & 0x0000000000FF0000) << 24) | ((value & 0x00000000FF000000) << 8) |
		   ((value & 0x000000FF00000000) >> 8) | ((value & 0x0000FF0000000000) >> 24) |
		   ((value & 0x00FF000000000000) >> 40) | ((value & 0xFF00000000000000) >> 56);
}

}  // namespace mfdasm::intops

#endif
