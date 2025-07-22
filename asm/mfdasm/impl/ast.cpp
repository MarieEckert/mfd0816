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
#include <numeric>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <shared/int_ops.hpp>
#include <shared/log.hpp>
#include <shared/panic.hpp>
#include <shared/typedefs.hpp>

#include <mfdasm/impl/ast.hpp>

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
	[[maybe_unused]] const ResolvalContext &resolval_context) const {
	return Ok(std::vector<u8>{});
}

ExpressionBase::ExpressionBase(Kind kind, u32 lineno) : m_lineno(lineno), m_kind(kind) {}

std::string ExpressionBase::toString([[maybe_unused]] u32 indentLevel) const {
	return "ExpressionBase\n";
}

u32 ExpressionBase::lineno() const {
	return m_lineno;
}

/* class StatementBase */

Result<std::vector<u8>, AsmError> StatementBase::toBytes(
	[[maybe_unused]] ResolvalContext &resolval_context) const {
	return Ok(std::vector<u8>{});
}

std::string StatementBase::toString([[maybe_unused]] u32 indentLevel) const {
	return "StatementBase\n";
}

const Expressions &StatementBase::expressions() const {
	return m_expressions;
}

u32 StatementBase::lineno() const {
	return m_lineno;
}

StatementBase::StatementBase(Expressions expressions, u32 lineno)
	: m_expressions(expressions), m_lineno(lineno) {}

/* class Literal */

Literal::Literal(std::vector<u8> value, u32 lineno)
	: ExpressionBase(ExpressionBase::LITERAL, lineno), m_value(value) {}

Result<std::vector<u8>, AsmError> Literal::resolveValue(
	[[maybe_unused]] const ResolvalContext &resolval_context) const {
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
		return Err(AsmError(AsmError::NO_SUCH_IDENTIFIER, m_lineno, m_name));
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

DirectAddress::DirectAddress(Kind kind, ExpressionPtr value_expression, u32 lineno)
	: ExpressionBase(ExpressionBase::DIRECT_ADDRESS, lineno),
	  m_kind(kind),
	  m_valueExpression(value_expression) {
	if(value_expression == nullptr) {
		shared::panic("DirectAddress constructed with a nullptr value_expression");
	}
}

Result<std::vector<u8>, AsmError> DirectAddress::resolveValue(
	const ResolvalContext &resolval_context) const {
	return m_valueExpression->resolveValue(resolval_context);
}

DirectAddress::Kind DirectAddress::kind() const {
	return m_kind;
}

std::string DirectAddress::toString(u32 indentLevel) const {
	const std::string base_indent = makeIndent(indentLevel);

	std::string expression_string = m_valueExpression->toString(indentLevel + 2);

	return base_indent + "DirectAddress {\n" + base_indent + "  kind: " + std::to_string(m_kind) +
		   "\n" + base_indent + "  expressions: \n" + expression_string + base_indent + "}\n";
}

/* class IndirectAddress */

IndirectAddress::IndirectAddress(Kind kind, ExpressionPtr value_expression, u32 lineno)
	: ExpressionBase(ExpressionBase::INDIRECT_ADDRESS, lineno),
	  m_kind(kind),
	  m_valueExpression(value_expression) {
	if(value_expression == nullptr) {
		shared::panic("IndirectAddress constructed with a nullptr value_expression");
	}
}

Result<std::vector<u8>, AsmError> IndirectAddress::resolveValue(
	const ResolvalContext &resolval_context) const {
	return m_valueExpression->resolveValue(resolval_context);
}

IndirectAddress::Kind IndirectAddress::kind() const {
	return m_kind;
}

std::string IndirectAddress::toString(u32 indentLevel) const {
	const std::string base_indent = makeIndent(indentLevel);

	std::string expression_string = m_valueExpression->toString(indentLevel + 2);

	return base_indent + "IndirectAddress {\n" + base_indent + "  kind: " + std::to_string(m_kind) +
		   "\n" + base_indent + "  expressions: \n" + expression_string + base_indent + "}\n";
}

/* class Register */

Register::Register(Kind kind, u32 lineno)
	: ExpressionBase(ExpressionBase::REGISTER, lineno), m_kind(kind) {}

Result<std::vector<u8>, AsmError> Register::resolveValue(
	[[maybe_unused]] const ResolvalContext &resolval_context) const {
	return Ok(
		std::vector<u8>{
			static_cast<u8>(m_kind),
		});
}

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
	case Token::AR:
		kind = Register::AR;
		break;
	case Token::IID:
		kind = Register::IID;
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

Instruction::Instruction(Kind kind, Expressions expressions, u32 lineno)
	: StatementBase(expressions, lineno), m_kind(kind) {
	if(Instruction::isReserved(kind)) {
		logWarning() << "Instruction constructed with reserved opcode 0x" << std::hex
					 << static_cast<i32>(kind) << std::dec << "\n";
	}
}

Result<std::vector<u8>, AsmError> Instruction::toBytes(ResolvalContext &resolval_context) const {
	std::vector<u8> result;

	result.push_back(m_kind);

	u8 operand_bits = 0;

	std::vector<u8> operands;

	for(usize ix = 0; ix < 2 && ix < m_expressions.size(); ix++) {
		bool reg = false;

		switch(m_expressions[ix]->kind()) {
		case ExpressionBase::IDENTIFIER:
		case ExpressionBase::LITERAL: {
			break;
		}
		case ExpressionBase::REGISTER: {
			operand_bits |= 0b1000;
			reg = true;
			break;
		}
		case ExpressionBase::DIRECT_ADDRESS: {
			reg = static_pointer_cast<DirectAddress>(m_expressions[ix])->kind() ==
				  DirectAddress::REGISTER;
			operand_bits |= 0b0001 | (static_cast<u8>(reg) * 0b1000);
			break;
		}
		case ExpressionBase::INDIRECT_ADDRESS: {
			reg = static_pointer_cast<IndirectAddress>(m_expressions[ix])->kind() ==
				  IndirectAddress::REGISTER;
			operand_bits |= 0b0010 | (static_cast<u8>(reg) * 0b1000);
			break;
		}
		case ExpressionBase::STATEMENT_EXPRESSION:
			shared::panic(
				"Instruction::toBytes() on Instruction with StatementExpression statement");
		}

		const Result<std::vector<u8>, AsmError> maybe_operand_bytes =
			m_expressions[ix]->resolveValue(resolval_context);
		if(maybe_operand_bytes.isErr()) {
			const AsmError err = maybe_operand_bytes.unwrapErr();
			if(err.type() != AsmError::NO_SUCH_IDENTIFIER) {
				return Err(err);
			}

			resolval_context.unresolvedIdentifiers.insert(
				{resolval_context.currentAddress + 2 + ix,
				 {.name = err.maybeMessage().value_or(""),
				  .width = static_cast<usize>(reg ? 1 : 2),
				  .lineno = m_expressions[ix]->lineno()}});
			operands.push_back(0);
			if(!reg) {
				operands.push_back(0);
			}
		} else {
			const std::vector<u8> operand_bytes = maybe_operand_bytes.unwrap();
			operands.insert(
				operands.end(), operand_bytes.end() - std::min<int>(operand_bytes.size(), 2),
				operand_bytes.end());
		}

		if(ix == 0) {
			operand_bits = operand_bits << 4;
		}
	}

	result.push_back(operand_bits);
	result.insert(result.end(), operands.begin(), operands.end());

	logDebug() << "instruction size: " << result.size() << "\n";
	for(const u8 b: result) {
		logDebug() << "\t" << std::hex << static_cast<int>(b) << "\n" << std::dec;
	}

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
	{Token::DEFINE, Directive::DEFINE}, {Token::DS, Directive::DS},
};

std::optional<Directive::Kind> Directive::kindFromToken(Token::Type type) {
	const auto res = directive_type_map.find(type);

	if(res == directive_type_map.cend()) {
		return std::nullopt;
	}

	return res->second;
}

Directive::Directive(Kind kind, Expressions expressions, u32 lineno)
	: StatementBase(expressions, lineno), m_kind(kind) {}

/** @todo implement */
Result<std::vector<u8>, AsmError> Directive::toBytes(ResolvalContext &resolval_context) const {
	switch(m_kind) {
	case Kind::DEFINE: {
		if(m_expressions.size() != 2) {
			shared::panic(
				"toBytes() on define directive with invalid expression count (expected 2, got " +
				std::to_string(m_expressions.size()) + ")");
		}

		const ExpressionPtr identifier_expr = m_expressions[0];
		const ExpressionPtr value_expr = m_expressions[1];

		if(identifier_expr->kind() != ExpressionBase::IDENTIFIER) {
			return Err(AsmError(
				AsmError::ILLEGAL_OPERAND, m_lineno,
				"First expression of define directive is not identifier"));
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
	case Kind::DB:
		return this->handleDefineNumberLiteral(resolval_context, 1);
	case Kind::DW:
		return this->handleDefineNumberLiteral(resolval_context, 2);
	case Kind::DD:
		return this->handleDefineNumberLiteral(resolval_context, 4);
	case Kind::DS:
		return this->handleDefineNumberLiteral(resolval_context, 0);
	case Kind::TIMES: {
		if(m_expressions.size() != 2) {
			shared::panic(
				"toBytes() on times directive with invalid expression count (expected 2, got " +
				std::to_string(m_expressions.size()) + ")");
		}

		const ExpressionPtr count_expr = m_expressions[0];
		const ExpressionPtr statement_expr = m_expressions[1];

		if(statement_expr->kind() != ExpressionBase::STATEMENT_EXPRESSION) {
			shared::panic("second expression of times directive is not a statement expression");
		}

		const Result<std::vector<u8>, AsmError> maybe_count =
			count_expr->resolveValue(resolval_context);
		if(maybe_count.isErr()) {
			return Err(maybe_count.unwrapErr());
		}

		const std::vector<u8> raw_count = maybe_count.unwrap();

		const u64 count =
			std::accumulate(raw_count.begin(), raw_count.end(), 0ll, shared::intops::accumulateU64);

		std::vector<u8> result;
		for(u64 ix = 0; ix < count; ix++) {
			const Result<std::vector<u8>, AsmError> statement_result =
				static_pointer_cast<StatementExpression>(statement_expr)
					->statement()
					.toBytes(resolval_context);
			if(statement_result.isErr()) {
				return Err(statement_result.unwrapErr());
			}

			const std::vector<u8> statement_bytes = statement_result.unwrap();
			result.insert(result.end(), statement_bytes.begin(), statement_bytes.end());
		}

		return Ok(result);
	}
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

void Directive::addExpression(ExpressionPtr expression) {
	m_expressions.push_back(expression);
}

Result<std::vector<u8>, AsmError> Directive::handleDefineNumberLiteral(
	ResolvalContext &resolval_context,
	usize width) const {
	if(m_expressions.size() != 1) {
		shared::panic(
			"handleDefineNumberLiteral() on directive with invalid expression count (expected 1, "
			"got " +
			std::to_string(m_expressions.size()) + ")");
	}

	Result<std::vector<u8>, AsmError> maybe_value =
		m_expressions[0]->resolveValue(resolval_context);
	if(maybe_value.isErr()) {
		return Err(maybe_value.unwrapErr());
	}

	std::vector<u8> value = maybe_value.unwrap();

	std::vector<u8> result;

	result.insert(
		result.begin(), std::prev(value.end(), width > 0 ? width : value.size()), value.end());

	return Ok(result);
}

/* class Statement */

Statement::Statement(
	Kind kind,
	Expressions expressions,
	u32 lineno,
	std::optional<StatementPtr> statement)
	: StatementBase(expressions, lineno), m_kind(kind), m_subStatement(std::move(statement)) {
	/* This is the result of a horrible decision and many instances of
	 * "eh, i'll fix it later"...
	 *
	 * many such cases
	 */
	if(!statement.has_value() && (kind == Kind::INSTRUCTION || kind == Kind::DIRECTIVE)) {
		shared::panic(
			"bad software design (kind = " + std::to_string(kind) + "; statement = nullptr)");
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
	switch(m_kind) {
	case Statement::LABEL:
		resolval_context.identifiers.emplace(
			static_pointer_cast<Identifier>(m_expressions[0])->name(),
			shared::intops::u64ToBytes(resolval_context.currentAddress));
	case Statement::ADDRESSING_ABSOLUTE:
	case Statement::ADDRESSING_RELATIVE:
	case Statement::SECTION:
		return Ok(std::vector<u8>{});
	case Statement::INSTRUCTION:
		if(!m_subStatement.has_value()) {
			shared::panic(
				"toBytes() called on Statement of kind INSTRUCTION without sub-statement");
		}
		return static_pointer_cast<Instruction>(m_subStatement.value())->toBytes(resolval_context);
	case Statement::DIRECTIVE:
		if(!m_subStatement.has_value()) {
			shared::panic("toBytes() called on Statement of kind DIRECTIVE without sub-statement");
		}
		return static_pointer_cast<Directive>(m_subStatement.value())->toBytes(resolval_context);
	}
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

std::optional<StatementPtr> Statement::maybeSubStatement() const {
	return m_subStatement;
}

/* class StatementExpression */

StatementExpression::StatementExpression(Statement statement, u32 lineno)
	: ExpressionBase(ExpressionBase::STATEMENT_EXPRESSION, lineno), m_statement(statement) {}

Statement StatementExpression::statement() {
	return m_statement;
}

std::string StatementExpression::toString(u32 indent_level) const {
	const std::string base_indent = makeIndent(indent_level);

	return base_indent + "StatementExpression {\n" + m_statement.toString(indent_level + 1) +
		   base_indent + "}\n";
}

}  // namespace mfdasm::impl
