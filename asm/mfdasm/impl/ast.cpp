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
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
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

ExpressionBase::ExpressionBase(Kind kind) : m_kind(kind) {}

std::string ExpressionBase::toString() const {
	return "ExpressionBase\n";
}

/* class StatementBase */

std::vector<u8> StatementBase::toBytes(ResolvalContext &resolval_context) const {
	return {0};
}

std::string StatementBase::toString() const {
	return "StatementBase\n";
}

StatementBase::StatementBase(std::vector<std::shared_ptr<ExpressionBase>> expressions)
	: m_expressions(expressions) {}

/* class Literal */

Literal::Literal(std::vector<u8> value) : ExpressionBase(ExpressionBase::LITERAL), m_value(value) {}

std::string Literal::toString() const {
	return "LITERAL\n";
}

/* class Identifier */

Identifier::Identifier(Kind kind, std::string name)
	: ExpressionBase(ExpressionBase::IDENTIFIER), m_kind(kind), m_name(name) {}

std::string Identifier::toString() const {
	return "Identifier\n";
}

/* class DirectAddress */

DirectAddress::DirectAddress(Kind kind, std::vector<u8> value)
	: ExpressionBase(ExpressionBase::DIRECT_ADDRESS), m_kind(kind), m_value(value) {}

DirectAddress::Kind DirectAddress::kind() const {
	return this->m_kind;
}

std::string DirectAddress::toString() const {
	return "DirectAddress\n";
}

/* class IndirectAddress */

IndirectAddress::IndirectAddress(Kind kind, std::vector<u8> value)
	: ExpressionBase(ExpressionBase::INDIRECT_ADDRESS), m_kind(kind), m_value(value) {}

IndirectAddress::Kind IndirectAddress::kind() const {
	return this->m_kind;
}

std::string IndirectAddress::toString() const {
	return "IndirectAddress\n";
}

/* class Register */

Register::Register(Kind kind) : ExpressionBase(ExpressionBase::REGISTER), m_kind(kind) {}

std::optional<Register> Register::fromTokenType(Token::Type type) {
	switch(type) {
		case Token::SP:
			return Register(Register::SP);
		case Token::IP:
			return Register(Register::IP);
		case Token::FL:
			return Register(Register::FL);
		case Token::AL:
			return Register(Register::AL);
		case Token::AH:
			return Register(Register::AH);
		case Token::ACL:
			return Register(Register::ACL);
		case Token::BL:
			return Register(Register::BL);
		case Token::BH:
			return Register(Register::BH);
		case Token::BCL:
			return Register(Register::BCL);
		case Token::CL:
			return Register(Register::CL);
		case Token::CH:
			return Register(Register::CH);
		case Token::CCL:
			return Register(Register::CCL);
		case Token::DL:
			return Register(Register::DL);
		case Token::DH:
			return Register(Register::DH);
		case Token::DCL:
			return Register(Register::DCL);
		default:
			return std::nullopt;
	}
}

Register::Kind Register::kind() const {
	return this->m_kind;
}

std::string Register::toString() const {
	return "Register\n";
}

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

static const std::unordered_map<std::string, Instruction::Kind> instruction_name_map = {
	{"adc", Instruction::ADC},	 {"add", Instruction::ADD},	  {"and", Instruction::AND},
	{"bin", Instruction::BIN},	 {"bot", Instruction::BOT},	  {"call", Instruction::CALL},
	{"cmp", Instruction::CMP},	 {"dec", Instruction::DEC},	  {"div", Instruction::DIV},
	{"idiv", Instruction::IDIV}, {"imul", Instruction::IMUL}, {"in", Instruction::IN},
	{"inc", Instruction::INC},	 {"int", Instruction::INT},	  {"iret", Instruction::IRET},
	{"jmp", Instruction::JMP},	 {"jz", Instruction::JZ},	  {"jg", Instruction::JG},
	{"jge", Instruction::JGE},	 {"jl", Instruction::JL},	  {"jle", Instruction::JLE},
	{"jc", Instruction::JC},	 {"js", Instruction::JS},	  {"jnz", Instruction::JNZ},
	{"jnc", Instruction::JNC},	 {"jns", Instruction::JNS},	  {"ld", Instruction::LD},
	{"mov", Instruction::MOV},	 {"mul", Instruction::MUL},	  {"neg", Instruction::NEG},
	{"nop", Instruction::NOP},	 {"not", Instruction::NOT},	  {"or", Instruction::OR},
	{"out", Instruction::OUT},	 {"pop", Instruction::POP},	  {"push", Instruction::PUSH},
	{"ret", Instruction::RET},	 {"rol", Instruction::ROL},	  {"ror", Instruction::ROR},
	{"sl", Instruction::SL},	 {"sr", Instruction::SR},	  {"st", Instruction::ST},
	{"clo", Instruction::CLO},	 {"clc", Instruction::CLC},	  {"clz", Instruction::CLZ},
	{"cln", Instruction::CLN},	 {"cli", Instruction::CLI},	  {"sto", Instruction::STO},
	{"stc", Instruction::STC},	 {"stz", Instruction::STZ},	  {"stn", Instruction::STN},
	{"sti", Instruction::STI},	 {"sub", Instruction::SUB},	  {"test", Instruction::TEST},
	{"xor", Instruction::XOR},
};

std::optional<Instruction::Kind> Instruction::kindFromString(const std::string &str) {
	const auto res = instruction_name_map.find(str);

	if(res == instruction_name_map.cend()) {
		return std::nullopt;
	}

	return res->second;
}

Instruction::Instruction(Kind kind, std::vector<std::shared_ptr<ExpressionBase>> expressions)
	: StatementBase(expressions), m_kind(kind) {
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
	std::string expression_string = "";
	for(const auto &expression: this->m_expressions) {
		expression_string += expression->toString();
	}

	return "Instruction {\n"
		   "  kind: " +
		   std::to_string(this->m_kind) +
		   "\n"
		   "  expressions: [" +
		   expression_string +
		   "  ]\n"
		   "}";
}

/* class Directive */

static const std::unordered_map<std::string, Directive::Kind> directive_name_map = {
	{"db", Directive::DB},
	{"dw", Directive::DW},
	{"dd", Directive::DD},
	{"times", Directive::TIMES},
};

std::optional<Directive::Kind> Directive::kindFromString(const std::string &str) {
	const auto res = directive_name_map.find(str);

	if(res == directive_name_map.cend()) {
		return std::nullopt;
	}

	return res->second;
}

std::string Directive::toString() const {
	return "Directive";
}

/* class Statement */

Statement::Statement(
	Kind kind,
	std::vector<std::shared_ptr<ExpressionBase>> expressions,
	std::optional<std::shared_ptr<StatementBase>> statement)
	: StatementBase(expressions), m_kind(kind), m_subStatement(std::move(statement)) {
	if(!statement.has_value() && (kind == Kind::INSTRUCTION || kind == Kind::DIRECTIVE)) {
		panic("bad software design (kind = " + std::to_string(kind) + "; statement = nullptr)");
	}
}

Statement::Statement(Statement &x)
	: StatementBase(x.m_expressions), m_kind(x.m_kind), m_subStatement(x.m_subStatement) {}

Statement::Statement(Statement &&x)
	: StatementBase(std::move(x.m_expressions)),
	  m_kind(std::exchange(x.m_kind, static_cast<Statement::Kind>(0))),
	  m_subStatement(std::move(x.m_subStatement)) {}

Statement::Kind Statement::kind() const {
	return this->m_kind;
}

std::string Statement::toString() const {
	std::string expression_string = "";
	for(const auto &expression: this->m_expressions) {
		expression_string += expression->toString();
	}

	std::string statement_string =
		this->m_subStatement.has_value() ? this->m_subStatement->get()->toString() : "";

	return "Statement {\n"
		   "  kind: " +
		   std::to_string(this->m_kind) +
		   "\n"
		   "  expressions: [\n" +
		   expression_string +
		   "  ]\n"
		   "  statement: " +
		   statement_string +
		   "\n"
		   "}\n";
}

std::vector<u8> Statement::toBytes(ResolvalContext &resolval_context) const {
	return {};
}

}  // namespace mfdasm::impl