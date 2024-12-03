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

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <mfdasm/impl/ast.hpp>
#include <mfdasm/log.hpp>
#include <mfdasm/panic.hpp>
#include <mfdasm/typedefs.hpp>

namespace mfdasm::impl {

/* class ExpressionBase */

ExpressionBase::Kind ExpressionBase::kind() const {
	return this->m_kind;
}

/* class Literal */

Literal::Literal(std::vector<u8> value) : m_value(value) {}

/* class Identifier */

Identifier::Identifier(Kind kind, std::string name) : m_kind(kind), m_name(name) {}

/* class Instruction */

bool Instruction::isReserved(Kind kind) {
	switch(kind) {
		case _RESERVED_00:
		case _RESERVED_01:
		case _RESERVED_02:
		case _RESERVED_03:
		case _RESERVED_04:
		case _RESERVED_05:
		case _RESERVED_06:
		case _RESERVED_07:
		case _RESERVED_08:
		case _RESERVED_09:
		case _RESERVED_10:
		case _RESERVED_11:
		case _RESERVED_12:
		case _RESERVED_13:
		case _RESERVED_14:
		case _RESERVED_15:
		case _RESERVED_16:
		case _RESERVED_17:
		case _RESERVED_18:
		case _RESERVED_19:
		case _RESERVED_20:
		case _RESERVED_21:
		case _RESERVED_22:
			return true;
		default:
			return false;
	}
}

Instruction::Instruction(Kind kind, std::vector<ExpressionBase> expressions)
	: m_kind(kind), m_expressions(expressions) {
	if(Instruction::isReserved(kind)) {
		logWarning() << "Instruction constructed with reserved opcode 0x" << std::hex
					 << static_cast<i32>(kind) << std::dec << "\n";
	}
}

std::vector<u8> Instruction::toBytes(ResolvalContext &resolval_context) const {
	std::vector<u8> result;
	result.reserve(4);

	result.push_back(this->m_kind);

	return result;
}

std::string Instruction::toString() const {
	return "";
}

/* class Statement */

Statement::Statement(
	Kind kind,
	std::vector<ExpressionBase> expressions,
	std::unique_ptr<StatementBase> statement)
	: m_kind(kind), m_expressions(expressions), m_subStatement(std::move(statement)) {
	if(statement == nullptr && (kind == Kind::INSTRUCTION || kind == Kind::DIRECTIVE)) {
		panic("bad software design (kind = " + std::to_string(kind) + "; statement = nullptr)");
	}
}

Statement::Kind Statement::kind() const {
	return this->m_kind;
}

std::string Statement::toString() const {
	return "";
}

std::vector<u8> Statement::toBytes(ResolvalContext &resolval_context) const {
	return {};
}

}  // namespace mfdasm::impl