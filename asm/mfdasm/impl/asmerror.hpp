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

#ifndef MFDASM_IMPL_ASMERROR_HPP
#define MFDASM_IMPL_ASMERROR_HPP

#include <optional>
#include <string>

#include <mfdasm/typedefs.hpp>

namespace mfdasm::impl {
class AsmError {
   public:
	enum Type : u8 {
		EMPTY_INPUT,
		SYNTAX_ERROR,
		INVALID_INSTRUCTION,
		ILLEGAL_OPERAND,
	};

	static std::string errorName(Type type);

	AsmError(Type type, u32 lineno, std::optional<std::string> message = std::nullopt);

	/**
	 * @brief Create a readable string representation of the error.
	 * If m_message is not nullopt, it will also be appended to the end.
	 */
	std::string toString() const;

	Type type() const;

	u32 lineno() const;

	std::optional<std::string> maybeMessage() const;

   private:
	Type m_type;

	u32 m_lineno;

	std::optional<std::string> m_message;
};
}  // namespace mfdasm::impl

#endif
