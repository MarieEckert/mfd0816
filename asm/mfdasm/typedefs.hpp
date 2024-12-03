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

/**
 * @file typedefs.hpp
 * @brief Shared integer type defintions for MFDASM.
 */

#ifndef MFDASM_TYPEDEFS
#define MFDASM_TYPEDEFS

#include <cstddef>
#include <cstdint>

#include <sys/types.h>

using usize = std::size_t;
using isize = ssize_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
using i8 = std::int8_t;
using iFast16 = std::int_fast16_t;
using iFast32 = std::int_fast32_t;
using iFast64 = std::int_fast64_t;
using iFast8 = std::int_fast8_t;
using iLeast16 = std::int_least16_t;
using iLeast32 = std::int_least32_t;
using iLeast64 = std::int_least64_t;
using iLeast8 = std::int_least8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using u8 = std::uint8_t;
using uFast16 = std::uint_fast16_t;
using uFast32 = std::uint_fast32_t;
using uFast64 = std::uint_fast64_t;
using uFast8 = std::uint_fast8_t;
using uLeast16 = std::uint_least16_t;
using uLeast32 = std::uint_least32_t;
using uLeast64 = std::uint_least64_t;
using uLeast8 = std::uint_least8_t;

#endif