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

#ifndef MFDASM_IMPL_TOKEN_HPP
#define MFDASM_IMPL_TOKEN_HPP

#include <optional>
#include <string>
#include <vector>

#include <shared/typedefs.hpp>

namespace mfdasm::impl {
class Token {
   public:
	enum Type : u8 {
		/* misc */
		END_OF_FILE,
		UNKNOWN,
		LABEL,

		/* characters */

		COMMA,
		SINGLE_QUOTE, /* used for characters */
		DOUBLE_QUOTE, /* used for opening/closing strings */
		SEMICOLON,	  /* used for comments */
		LEFT_SQUARE_BRACKET,
		RIGHT_SQUARE_BRACKET,
		MINUS,
		PLUS,
		SLASH,
		STAR,

		/* reserved keywords */
		DB,
		DW,
		DD,
		TIMES,
		_HERE,	// NOLINT
		SECTION,
		SP,
		IP,
		FL,
		AL,
		AH,
		ACL,
		BL,
		BH,
		BCL,
		CL,
		CH,
		CCL,
		DL,
		DH,
		DCL,
		IID,
		AR,
		ADDRESSING,
		RELATIVE,
		ABSOLUTE,
		AT,
		DEFINE,
		DS,

		/* data literals */
		BINARY_NUMBER,
		DECIMAL_NUMBER,
		HEXADECIMAL_NUMBER,
		STRING,
	};

	static Type typeFromString(const std::string &str);

	static bool isNumberType(Type type);

	static bool isRegister(Type type);

	static int numberTypeBase(Type type);

	Token(Type type, u32 lineno, std::optional<std::string> value = std::nullopt);

	Type type() const;

	u32 lineno() const;

	std::optional<std::string> maybeValue() const;

	std::string toString() const;

	std::vector<u8> toBytes() const;

   private:
	Type m_type;

	u32 m_lineno;

	std::optional<std::string> m_maybeValue;
};
}  // namespace mfdasm::impl

#endif
