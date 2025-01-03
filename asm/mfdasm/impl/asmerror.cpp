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

#include <iomanip>
#include <ios>
#include <sstream>
#include <unordered_map>

#include <mfdasm/impl/asmerror.hpp>

namespace mfdasm::impl {

static const std::unordered_map<AsmError::Type, std::string> error_names = {
	{AsmError::SYNTAX_ERROR, "Syntax Error"},
	{AsmError::ILLEGAL_OPERAND, "Illegal Operand"},
	{AsmError::INVALID_INSTRUCTION, "Invalid Instruction"},
};

std::string AsmError::errorName(Type type) {
	return error_names.find(type)->second;
}

AsmError::AsmError(Type type, u32 lineno, std::optional<std::string> message)
	: m_type(type), m_lineno(lineno), m_message(message) {}

std::string AsmError::toString() const {
	std::stringstream ss;
	ss << errorName(m_type) << " (0x" << std::hex << std::setfill('0') << std::setw(2)
	   << static_cast<i32>(m_type) << ") on line " << std::dec << m_lineno
	   << (m_message.has_value() ? ": " + m_message.value() : "");
	return ss.str();
}

AsmError::Type AsmError::type() const {
	return m_type;
}

u32 AsmError::lineno() const {
	return m_lineno;
}

std::optional<std::string> AsmError::maybeMessage() const {
	return m_message;
}

}  // namespace mfdasm::impl