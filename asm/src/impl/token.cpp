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

#include <impl/token.hpp>
#include <optional>
#include <string>

namespace sasm::impl {

Token::Token(Type type, u32 lineno, std::optional<std::string> value)
	: m_type(type), m_lineno(lineno), m_maybeValue(value) {}

Token::Type Token::type() const {
	return this->m_type;
}

u32 Token::lineno() const {
	return this->m_lineno;
}

std::optional<std::string> Token::maybeValue() const {
	return this->m_maybeValue;
}

}  // namespace sasm::impl