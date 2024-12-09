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

#include <algorithm>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <mfdasm/impl/token.hpp>

namespace mfdasm::impl {

static inline const std::unordered_map<std::string, Token::Type> name_value_map = {
	{"end_of_file", Token::Type::END_OF_FILE},
	{"unknown", Token::Type::UNKNOWN},
	{"label", Token::Type::LABEL},
	{"comma", Token::Type::COMMA},
	{"single_quote", Token::Type::SINGLE_QUOTE},
	{"double_quote", Token::Type::DOUBLE_QUOTE},
	{"semicolon", Token::Type::SEMICOLON},
	{"left_square_bracket", Token::Type::LEFT_SQUARE_BRACKET},
	{"right_square_bracket", Token::Type::RIGHT_SQUARE_BRACKET},
	{"minus", Token::Type::MINUS},
	{"plus", Token::Type::PLUS},
	{"slash", Token::Type::SLASH},
	{"star", Token::Type::STAR},
	{"db", Token::Type::DB},
	{"dw", Token::Type::DW},
	{"dd", Token::Type::DD},
	{"times", Token::Type::TIMES},
	{"_here", Token::Type::_HERE},
	{"section", Token::Type::SECTION},
	{"sp", Token::Type::SP},
	{"ip", Token::Type::IP},
	{"fl", Token::Type::FL},
	{"al", Token::Type::AL},
	{"ah", Token::Type::AH},
	{"acl", Token::Type::ACL},
	{"bl", Token::Type::BL},
	{"bh", Token::Type::BH},
	{"bcl", Token::Type::BCL},
	{"cl", Token::Type::CL},
	{"ch", Token::Type::CH},
	{"ccl", Token::Type::CCL},
	{"dl", Token::Type::DL},
	{"dh", Token::Type::DH},
	{"dcl", Token::Type::DCL},
	{"addressing", Token::Type::ADDRESSING},
	{"relative", Token::Type::RELATIVE},
	{"absolute", Token::Type::ABSOLUTE},
	{"at", Token::Type::AT},
	{"binary_number", Token::Type::BINARY_NUMBER},
	{"decimal_number", Token::Type::DECIMAL_NUMBER},
	{"hexadecimal_number", Token::Type::HEXADECIMAL_NUMBER},
	{"string", Token::Type::STRING},
};

Token::Type Token::typeFromString(const std::string &str) {
	if(std::isdigit(str[0])) {
		if(str.length() == 1) {
			return Type::DECIMAL_NUMBER;
		}

		/* determine base, supported: base 16 (0x), base 10, base 2 (0b) */
		switch(str[1]) {
			case 'x':
				return Type::HEXADECIMAL_NUMBER;
			case 'b':
				return Type::BINARY_NUMBER;
			default:
				return Type::DECIMAL_NUMBER;
		}
	}

	if(str.back() == ':') {
		return Type::LABEL;
	}

	const auto it = std::find_if(
		name_value_map.cbegin(), name_value_map.cend(),
		[str](std::pair<std::string, Type> val) { return val.first == str; });

	return it != name_value_map.cend() ? it->second : Type::UNKNOWN;
}

bool Token::isNumberType(Type type) {
	switch(type) {
		case BINARY_NUMBER:
		case DECIMAL_NUMBER:
		case HEXADECIMAL_NUMBER:
			return true;
		default:
			return false;
	}
};

bool Token::isRegister(Type type) {
	switch(type) {
		case SP:
		case IP:
		case FL:
		case AL:
		case AH:
		case ACL:
		case BL:
		case BH:
		case BCL:
		case CL:
		case CH:
		case CCL:
		case DL:
		case DH:
		case DCL:
			return true;
		default:
			return false;
	}
}

int Token::numberTypeBase(Type type) {
	switch(type) {
		case BINARY_NUMBER:
			return 2;
		case DECIMAL_NUMBER:
			return 10;
		case HEXADECIMAL_NUMBER:
			return 16;
		default:
			return 0;
	}
}

Token::Token(Type type, u32 lineno, std::optional<std::string> value)
	: m_type(type), m_lineno(lineno) {
	switch(m_type) {
		case Type::BINARY_NUMBER:
		case Type::DECIMAL_NUMBER:
		case Type::HEXADECIMAL_NUMBER:
		case Type::STRING:
		case Type::LABEL:
		case Type::UNKNOWN:
			this->m_maybeValue = value;
			break;
		default:
			break;
	}
}

Token::Type Token::type() const {
	return this->m_type;
}

u32 Token::lineno() const {
	return this->m_lineno;
}

std::optional<std::string> Token::maybeValue() const {
	return this->m_maybeValue;
}

std::string Token::toString() const {
	const auto it = std::find_if(
		name_value_map.cbegin(), name_value_map.cend(),
		[type = this->m_type](std::pair<std::string, Type> val) { return val.second == type; });
	const std::string type_name = it != name_value_map.cend() ? it->first : "unknown";

	const std::string value_string =
		this->m_maybeValue.has_value() ? "value = " + this->m_maybeValue.value() : "";

	return "[line = " + std::to_string(this->m_lineno) + "; type = " + type_name + "; " +
		   value_string + "]";
}

std::vector<u8> Token::toBytes() const {
	if(!this->m_maybeValue.has_value()) {
		return {};
	}

	if(Token::isNumberType(this->m_type)) {
		const u16 value =
			std::stoi(this->m_maybeValue.value(), 0, Token::numberTypeBase(this->m_type));

		return {static_cast<u8>((value >> 8) & 0xFF), static_cast<u8>(value & 0xFF)};
	}

	if(this->m_type == Token::STRING) {
		return std::vector<u8>(
			this->m_maybeValue.value().begin(), this->m_maybeValue.value().end());
	}

	return {};
}

}  // namespace mfdasm::impl