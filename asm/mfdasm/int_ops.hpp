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

}  // namespace mfdasm::intops

#endif
