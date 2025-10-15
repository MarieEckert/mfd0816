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
#include <utility>
#include <vector>

#include <shared/int_ops.hpp>
#include <shared/panic.hpp>

#include <mfdasm/impl/token.hpp>

namespace mfdasm::impl {

static const std::unordered_map<Token::Type, u8> register_value_map = {
	{Token::AL, 0x00},	{Token::AH, 0x01},	{Token::ACL, 0x02}, {Token::BL, 0x03},
	{Token::BH, 0x04},	{Token::BCL, 0x05}, {Token::CL, 0x06},	{Token::CH, 0x07},
	{Token::CCL, 0x08}, {Token::DL, 0x09},	{Token::DH, 0x0a},	{Token::DCL, 0x0b},
	{Token::SP, 0x0c},	{Token::IP, 0x0d},	{Token::AR, 0x0e},	{Token::FL, 0x0f},
	{Token::IID, 0x10},
};

static const std::unordered_map<std::string, Token::Type> name_value_map = {
	{"end_of_file", Token::Type::END_OF_FILE},
	{"unknown", Token::Type::UNKNOWN},
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
	{"iid", Token::Type::IID},
	{"ar", Token::Type::AR},
	{"addressing", Token::Type::ADDRESSING},
	{"relative", Token::Type::RELATIVE},
	{"absolute", Token::Type::ABSOLUTE},
	{"at", Token::Type::AT},
	{"define", Token::Type::DEFINE},
	{"ds", Token::Type::DS},
	{"binary_number", Token::Type::BINARY_NUMBER},
	{"decimal_number", Token::Type::DECIMAL_NUMBER},
	{"hexadecimal_number", Token::Type::HEXADECIMAL_NUMBER},
	{"string", Token::Type::STRING},
};

Token::Type Token::typeFromString(const std::string &str) {
	if(std::isdigit(str[0]) != 0) {
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

	const auto it = std::ranges::find_if(
		name_value_map,
		[str](const std::pair<std::string, Type> &val) { return val.first == str; });

	return it != name_value_map.cend() ? it->second : Type::UNKNOWN;
}

bool Token::isNumberType(Type type) {
	switch(type) {
	case Type::BINARY_NUMBER:
	case Type::DECIMAL_NUMBER:
	case Type::HEXADECIMAL_NUMBER:
		return true;
	default:
		return false;
	}
};

bool Token::isRegister(Type type) {
	switch(type) {
	case Type::SP:
	case Type::IP:
	case Type::FL:
	case Type::AL:
	case Type::AH:
	case Type::ACL:
	case Type::BL:
	case Type::BH:
	case Type::BCL:
	case Type::CL:
	case Type::CH:
	case Type::CCL:
	case Type::DL:
	case Type::DH:
	case Type::DCL:
	case Type::AR:
	case Type::IID:
		return true;
	default:
		return false;
	}
}

int Token::numberTypeBase(Type type) {
	switch(type) {
	case Type::BINARY_NUMBER:
		return 2;
	case Type::DECIMAL_NUMBER:
		return 10;
	case Type::HEXADECIMAL_NUMBER:
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
		m_maybeValue = std::move(value);
		break;
	default:
		break;
	}
}

Token::Type Token::type() const {
	return m_type;
}

u32 Token::lineno() const {
	return m_lineno;
}

std::optional<std::string> Token::maybeValue() const {
	return m_maybeValue;
}

std::string Token::toString() const {
	const auto it = std::ranges::find_if(
		name_value_map,
		[type = m_type](const std::pair<std::string, Type> &val) { return val.second == type; });
	const std::string type_name = it != name_value_map.cend() ? it->first : "unknown";

	const std::string value_string =
		m_maybeValue.has_value() ? "value = " + m_maybeValue.value() : "";

	return "[line = " + std::to_string(m_lineno) + "; type = " + type_name + "; " + value_string +
		   "]";
}

std::vector<u8> Token::toBytes() const {
	if(Token::isRegister(m_type)) {
		const auto iterator = register_value_map.find(m_type);
		if(iterator == register_value_map.end()) {
			shared::panic(
				"register missing in Token::register_value_map! (m_type =" +
				std::to_string(m_type) + ")");
		}

		return {iterator->second};
	}

	if(!m_maybeValue.has_value()) {
		return {};
	}

	if(Token::isNumberType(m_type)) {
		const u64 value = std::stoull(m_maybeValue.value(), nullptr, Token::numberTypeBase(m_type));

		return shared::intops::u64ToBytes(value);
	}

	if(m_type == Token::STRING) {
		return {m_maybeValue.value().begin(), m_maybeValue.value().end()};
	}

	return {};
}

}  // namespace mfdasm::impl
