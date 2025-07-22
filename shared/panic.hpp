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

#ifndef MFDASM_PANIC_HPP
#define MFDASM_PANIC_HPP

#include <string>

namespace shared {

extern std::string program_name;

/**
 * @brief Function used to (nearly) immediatly exit the program in case
 * something goes wrong. Will print the given error message alongside a
 * backtrace.
 * @param error The error message to print.
 */
[[noreturn]] void panic(const std::string &error);

} // namespace shared

#endif