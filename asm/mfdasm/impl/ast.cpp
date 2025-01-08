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
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <mfdasm/impl/ast.hpp>
#include <mfdasm/log.hpp>
#include <mfdasm/panic.hpp>
#include <mfdasm/typedefs.hpp>

namespace mfdasm::impl {

/** @todo :duck: */
static std::string makeIndent(u32 level) {
	std::string single = "  ";
	std::string result = "";

	for(u32 n = 0; n < level; n++) {
		result += single;
	}

	return result;
}

/* class ExpressionBase */

ExpressionBase::Kind ExpressionBase::kind() const {
	return m_kind;
}

Result<std::vector<u8>, AsmError> ExpressionBase::resolveValue(
	const ResolvalContext &resolval_context) const {
	return Ok(std::vector<u8>{});
}

ExpressionBase::ExpressionBase(Kind kind, u32 lineno) : m_kind(kind), m_lineno(lineno) {}

std::string ExpressionBase::toString([[maybe_unused]] u32 indentLevel) const {
	return "ExpressionBase\n";
}

/* class StatementBase */

Result<std::vector<u8>, AsmError> StatementBase::toBytes(
	[[maybe_unused]] ResolvalContext &resolval_context) const {
	return Ok(std::vector<u8>{});
}

std::string StatementBase::toString([[maybe_unused]] u32 indentLevel) const {
	return "StatementBase\n";
}

StatementBase::StatementBase(std::vector<std::shared_ptr<ExpressionBase>> expressions, u32 lineno)
	: m_expressions(expressions), m_lineno(lineno) {}

/* class Literal */

Literal::Literal(std::vector<u8> value, u32 lineno)
	: ExpressionBase(ExpressionBase::LITERAL, lineno), m_value(value) {}

Result<std::vector<u8>, AsmError> Literal::resolveValue(
	const ResolvalContext &resolval_context) const {
	return Ok(m_value);
}

std::string Literal::toString(u32 indentLevel) const {
	const std::string base_indent = makeIndent(indentLevel);

	std::string value_string;
	for(const u8 byte: m_value) {
		std::stringstream ss;
		ss << std::hex << (int)byte;
		value_string += "\n" + base_indent + "    0x" + ss.str() + ",";
	}

	return base_indent + "Literal{\n" + base_indent + "  value: [" + value_string + "\n" +
		   base_indent + "  ]\n" + base_indent + "}\n";
}

/* class Identifier */

Identifier::Identifier(Kind kind, std::string name, u32 lineno)
	: ExpressionBase(ExpressionBase::IDENTIFIER, lineno), m_kind(kind), m_name(name) {}

Result<std::vector<u8>, AsmError> Identifier::resolveValue(
	const ResolvalContext &resolval_context) const {
	std::vector<u8> value;

	const auto iterator = std::find_if(
		resolval_context.identifiers.cbegin(), resolval_context.identifiers.cend(),
		[identifier_name = m_name](
			std::pair<const std::basic_string<char>, std::vector<unsigned char>> map_value) {
			return map_value.first == identifier_name;
		});

	if(iterator == resolval_context.identifiers.cend()) {
		return Ok(std::vector<u8>{});
	}

	return Ok(iterator->second);
}

std::string Identifier::toString(u32 indentLevel) const {
	const std::string base_indent = makeIndent(indentLevel);

	return base_indent + "Identifier {\n" + base_indent + "  kind: " + std::to_string(m_kind) +
		   "\n" + base_indent + "  name: " + m_name + "\n" + base_indent + "}\n";
}

Identifier::Kind Identifier::kind() const {
	return m_kind;
}

std::string Identifier::name() const {
	return m_name;
}

/* class DirectAddress */

DirectAddress::DirectAddress(Kind kind, std::vector<u8> value, u32 lineno)
	: ExpressionBase(ExpressionBase::DIRECT_ADDRESS, lineno), m_kind(kind), m_value(value) {
	if(value.size() == 0) {
		panic("shit");
	}
}

DirectAddress::Kind DirectAddress::kind() const {
	return m_kind;
}

std::string DirectAddress::toString(u32 indentLevel) const {
	const std::string base_indent = makeIndent(indentLevel);
	std::string value_string;
	for(const u8 byte: m_value) {
		std::stringstream ss;
		ss << std::hex << (int)byte;
		value_string += "\n" + base_indent + "    0x" + ss.str() + ",";
	}
	return base_indent + "DirectAddress {\n" + base_indent + "  kind: " + std::to_string(m_kind) +
		   "\n" + base_indent + "  value: [" + value_string + "\n" + base_indent + "  ]\n" +
		   base_indent + "}\n";
}

/* class IndirectAddress */

IndirectAddress::IndirectAddress(Kind kind, std::vector<u8> value, u32 lineno)
	: ExpressionBase(ExpressionBase::INDIRECT_ADDRESS, lineno), m_kind(kind), m_value(value) {}

IndirectAddress::Kind IndirectAddress::kind() const {
	return m_kind;
}

std::string IndirectAddress::toString(u32 indentLevel) const {
	const std::string base_indent = makeIndent(indentLevel);
	std::string value_string;
	for(const u8 byte: m_value) {
		std::stringstream ss;
		ss << std::hex << (int)byte;
		value_string += "\n" + base_indent + "    0x" + ss.str() + ",";
	}
	return base_indent + "Indirect {\n" + base_indent + "  kind: " + std::to_string(m_kind) + "\n" +
		   base_indent + "  value: [" + value_string + "\n" + base_indent + "  ]\n" + base_indent +
		   "}\n";
}

/* class Register */

Register::Register(Kind kind, u32 lineno)
	: ExpressionBase(ExpressionBase::REGISTER, lineno), m_kind(kind) {}

std::optional<Register> Register::fromToken(const Token &token) {
	Register::Kind kind;
	switch(token.type()) {
		case Token::SP:
			kind = Register::SP;
			break;
		case Token::IP:
			kind = Register::IP;
			break;
		case Token::FL:
			kind = Register::FL;
			break;
		case Token::AL:
			kind = Register::AL;
			break;
		case Token::AH:
			kind = Register::AH;
			break;
		case Token::ACL:
			kind = Register::ACL;
			break;
		case Token::BL:
			kind = Register::BL;
			break;
		case Token::BH:
			kind = Register::BH;
			break;
		case Token::BCL:
			kind = Register::BCL;
			break;
		case Token::CL:
			kind = Register::CL;
			break;
		case Token::CH:
			kind = Register::CH;
			break;
		case Token::CCL:
			kind = Register::CCL;
			break;
		case Token::DL:
			kind = Register::DL;
			break;
		case Token::DH:
			kind = Register::DH;
			break;
		case Token::DCL:
			kind = Register::DCL;
			break;
		default:
			return std::nullopt;
	}

	return Register(kind, token.lineno());
}

Register::Kind Register::kind() const {
	return m_kind;
}

std::string Register::toString(u32 indentLevel) const {
	const std::string base_indent = makeIndent(indentLevel);
	return base_indent + "Register {\n" + base_indent + "  kind: " + std::to_string(m_kind) + "\n" +
		   base_indent + "}\n";
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

Instruction::Instruction(
	Kind kind,
	std::vector<std::shared_ptr<ExpressionBase>> expressions,
	u32 lineno)
	: StatementBase(expressions, lineno), m_kind(kind) {
	if(Instruction::isReserved(kind)) {
		logWarning() << "Instruction constructed with reserved opcode 0x" << std::hex
					 << static_cast<i32>(kind) << std::dec << "\n";
	}
}

Result<std::vector<u8>, AsmError> Instruction::toBytes(ResolvalContext &resolval_context) const {
	std::vector<u8> result;
	result.reserve(4);

	result.push_back(m_kind);

	return Ok(result);
}

std::string Instruction::toString(u32 indentLevel) const {
	const std::string base_indent = makeIndent(indentLevel);

	std::string expression_string = "";
	for(const auto &expression: m_expressions) {
		expression_string += expression->toString(indentLevel + 2);
	}

	return "Instruction {\n" + base_indent + "  kind: " + std::to_string(m_kind) + "\n" +
		   base_indent + "  expressions: [\n" + expression_string + base_indent + "  ]\n" +
		   base_indent + "}";
}

/* class Directive */

static const std::unordered_map<Token::Type, Directive::Kind> directive_type_map = {
	{Token::DB, Directive::DB},			{Token::DW, Directive::DW},
	{Token::DD, Directive::DD},			{Token::TIMES, Directive::TIMES},
	{Token::DEFINE, Directive::DEFINE},
};

std::optional<Directive::Kind> Directive::kindFromToken(Token::Type type) {
	const auto res = directive_type_map.find(type);

	if(res == directive_type_map.cend()) {
		return std::nullopt;
	}

	return res->second;
}

Directive::Directive(
	Kind kind,
	std::vector<std::shared_ptr<ExpressionBase>> expressions,
	u32 lineno)
	: StatementBase(expressions, lineno), m_kind(kind) {}

/** @todo implement */
Result<std::vector<u8>, AsmError> Directive::toBytes(ResolvalContext &resolval_context) const {
	if(m_kind == Kind::DEFINE) {
		if(m_expressions.size() != 2) {
			panic(
				"toBytes() on Directive with invalid expression count (expected 2, got " +
				std::to_string(m_expressions.size()) + ")");
		}

		const std::shared_ptr<ExpressionBase> identifier_expr = m_expressions[0];
		const std::shared_ptr<ExpressionBase> value_expr = m_expressions[1];

		if(identifier_expr->kind() != ExpressionBase::IDENTIFIER) {
			return Err(AsmError(
				AsmError::ILLEGAL_OPERAND, m_lineno,
				"First expression of directive is not identifier"));
		}

		const Identifier *identifier = dynamic_cast<Identifier *>(identifier_expr.get());

		Result<std::vector<u8>, AsmError> maybe_value = value_expr->resolveValue(resolval_context);
		if(maybe_value.isErr()) {
			return Err(AsmError(
				AsmError::VALUE_RESOLVAL_ERROR, m_lineno, "could not resolve value of expression"));
		}

		resolval_context.identifiers.emplace(identifier->name(), maybe_value.unwrap());
		return Ok(std::vector<u8>{});
	}

	return Ok(std::vector<u8>{});
}

std::string Directive::toString(u32 indentLevel) const {
	const std::string base_indent = makeIndent(indentLevel);

	std::string expression_string = "";
	for(const auto &expression: m_expressions) {
		expression_string += expression->toString(indentLevel + 2);
	}

	return "Directive {\n" + base_indent + "  kind: " + std::to_string(m_kind) + "\n" +
		   base_indent + "  expressions: [\n" + expression_string + base_indent + "  ]\n" +
		   base_indent + "}";
}

/* class Statement */

Statement::Statement(
	Kind kind,
	std::vector<std::shared_ptr<ExpressionBase>> expressions,
	u32 lineno,
	std::optional<std::shared_ptr<StatementBase>> statement)
	: StatementBase(expressions, lineno), m_kind(kind), m_subStatement(std::move(statement)) {
	/* This is the result of a horrible decision and many instances of
	 * "eh, i'll fix it later"...
	 *
	 * many such cases
	 */
	if(!statement.has_value() && (kind == Kind::INSTRUCTION || kind == Kind::DIRECTIVE)) {
		panic("bad software design (kind = " + std::to_string(kind) + "; statement = nullptr)");
	}
}

Statement::Statement(Statement &x)
	: StatementBase(x.m_expressions, x.m_lineno),
	  m_kind(x.m_kind),
	  m_subStatement(x.m_subStatement) {}

Statement::Statement(Statement &&x)
	: StatementBase(std::move(x.m_expressions), x.m_lineno),
	  m_kind(std::exchange(x.m_kind, static_cast<Statement::Kind>(0))),
	  m_subStatement(std::move(x.m_subStatement)) {}

Statement::Kind Statement::kind() const {
	return m_kind;
}

/** @todo implement */
Result<std::vector<u8>, AsmError> Statement::toBytes(ResolvalContext &resolval_context) const {
	return Ok(std::vector<u8>{});
}

std::string Statement::toString(u32 indentLevel) const {
	std::string expression_string = "";

	for(const auto &expression: m_expressions) {
		expression_string += expression->toString(indentLevel + 2);
	}

	std::string statement_string = m_subStatement.has_value() && m_subStatement.value() != nullptr
									   ? m_subStatement->get()->toString(indentLevel + 1)
									   : "std::nullopt / nullptr";

	const std::string base_indent = makeIndent(indentLevel);

	return base_indent + "Statement {\n" + base_indent + "  kind: " + std::to_string(m_kind) +
		   "\n" + base_indent + "  expressions: [\n" + expression_string + base_indent + "  ]\n" +
		   base_indent + "  statement: " + statement_string + base_indent + "\n" + base_indent +
		   "}\n";
}

}  // namespace mfdasm::impl
