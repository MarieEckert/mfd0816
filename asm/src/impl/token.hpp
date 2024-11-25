/* This file is part of MFDASM.
 *
 * MFDASM is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * MFDASM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * MFDASM. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SASM_IMPL_TOKEN_HPP
#define SASM_IMPL_TOKEN_HPP

#include <algorithm>
#include <optional>
#include <string>

#include <typedefs.hpp>
#include <unordered_map>

namespace sasm::impl {
class Token {
   public:
	enum Type {
		END_OF_FILE,
		UNKNOWN,
		COMMA,
		DOUBLE_QUOTE,
		SEMICOLON,

		/* reserved keywords */
		DB,
		DW,
		DD,
		TIMES,
		_HERE,
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
		ADDRESSING,
		RELATIVE,
		ABSOLUTE,
		AT
	};

	static inline Type typeFromString(const std::string &str) {
		static const std::unordered_map<std::string, Type> vals = {
			{"comma", COMMA},
			{"double_quote", DOUBLE_QUOTE},
			{";", SEMICOLON},
			{"db", DB},
			{"dw", DW},
			{"dd", DD},
			{"times", TIMES},
			{"_here", _HERE},
			{"section", SECTION},
			{"sp", SP},
			{"ip", IP},
			{"fl", FL},
			{"al", AL},
			{"ah", AH},
			{"acl", ACL},
			{"bl", BL},
			{"bh", BH},
			{"bcl", BCL},
			{"cl", CL},
			{"ch", CH},
			{"ccl", CCL},
			{"dl", DL},
			{"dh", DH},
			{"dcl", DCL},
			{"addressing", ADDRESSING},
			{"relative", RELATIVE},
			{"absolute", ABSOLUTE},
			{"at", AT},
		};

		const auto it = std::find_if(
			vals.cbegin(), vals.cend(),
			[str](std::pair<std::string, Type> val) { return val.first == str; });
		return it != vals.cend() ? it->second : Type::UNKNOWN;
	}

	Token(Type type, u32 lineno, std::optional<std::string> value = std::nullopt);

	Type type() const;

	u32 lineno() const;

	std::optional<std::string> maybeValue() const;

   private:
	Type m_type;

	u32 m_lineno;

	std::optional<std::string> m_maybeValue;
};
}  // namespace sasm::impl

#endif