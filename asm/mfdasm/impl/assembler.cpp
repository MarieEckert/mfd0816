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

#include <3rdparty/map.hpp>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include <shared/log.hpp>
#include <shared/panic.hpp>

#include <mfdasm/impl/asmerror.hpp>
#include <mfdasm/impl/assembler.hpp>
#include <mfdasm/impl/ast.hpp>
#include <mfdasm/impl/directive_operand.hpp>
#include <mfdasm/impl/instruction_operand.hpp>
#include <mfdasm/impl/token.hpp>

namespace mfdasm::impl {

static bool isCharReserved(char c) {
	switch(c) {
	case '[':
	case ']':
	case '-':
	case '+':
	case '*':
	case '/':
	case '\'':
	case '"':
	case ',':
		return true;
	default:
		return false;
	}
}

/* class Assembler */

Result<None, AsmError> Assembler::parseLines(const std::string &source) {
	if(source.empty()) {
		return Ok(None());
	}

	Result<std::vector<Token>, AsmError> lex_result = Lexer::process(source);
	if(lex_result.isErr()) {
		return Err(lex_result.unwrapErr());
	}

	Result<Parser, AsmError> parse_result = Parser::process(lex_result.unwrap());
	if(parse_result.isErr()) {
		return Err(parse_result.unwrapErr());
	}

	m_ast = parse_result.unwrap().ast();

	return Ok(None());
}

Result<mri::SectionTable, AsmError> Assembler::astToBytes() const {
	mri::SectionTable section_table;
	std::shared_ptr<mri::Section> current_section = nullptr;
	ResolvalContext resolval_context;

	for(const Statement &statement: m_ast) {
		if(statement.kind() == Statement::SECTION) {
			if(current_section != nullptr) {
				logDebug() << "old section contains "
						   << std::to_string(current_section->data.size()) << " bytes\n";
			}
			Result<std::shared_ptr<mri::Section>, AsmError> new_section =
				section_table.addFromStatement(statement, resolval_context);
			if(new_section.isErr()) {
				return Err(new_section.unwrapErr());
			}

			current_section = new_section.unwrap();
			logInfo() << "new section at offset " << current_section->offset << "\n";
			resolval_context.currentAddress = current_section->offset;
			continue;
		}

		if(statement.kind() == Statement::ADDRESSING_RELATIVE ||
		   statement.kind() == Statement::ADDRESSING_ABSOLUTE) {
			logWarning() << "addressing statements are not fully implemented yet!\n";
			continue;
		}

		if(current_section == nullptr) {
			logWarning() << "code on line " << std::to_string(statement.lineno())
						 << " outside any section.\n";
			logWarning() << "  -> This code will be ignored\n";
			continue;
		}

		Result<std::vector<u8>, AsmError> to_bytes_result = statement.toBytes(resolval_context);
		if(to_bytes_result.isErr()) {
			return Err(to_bytes_result.unwrapErr());
		}

		std::vector<u8> bytes = to_bytes_result.unwrap();
		current_section->data.insert(current_section->data.end(), bytes.begin(), bytes.end());
		resolval_context.currentAddress += bytes.size();
	}

	/* resolve missing */

	for(const auto &entry: resolval_context.unresolvedIdentifiers) {
		const usize pos = entry.first;
		const UnresolvedIdentifier unresolved_identifier = entry.second;

		logDebug() << "unresolved identifier: pos = " << entry.first
				   << "; width = " << unresolved_identifier.width << "; name = \""
				   << unresolved_identifier.name << "\";\n";
		const auto identifier = resolval_context.identifiers.find(unresolved_identifier.name);
		if(identifier == resolval_context.identifiers.end()) {
			return Err(AsmError(
				AsmError::NO_SUCH_IDENTIFIER, unresolved_identifier.lineno,
				unresolved_identifier.name));
		}

		const std::shared_ptr<mri::Section> section = section_table.findSectionByAddress(pos);
		if(section == nullptr) {
			shared::panic(
				"no matching section for pos of unresolved identifier (pos = " +
				std::to_string(pos) + ")");
		}

		std::copy(
			std::prev(identifier->second.end(), unresolved_identifier.width),
			identifier->second.end(), std::next(section->data.begin(), pos - section->offset));
	}

	return Ok(section_table);
}

std::optional<std::vector<Statement>> Assembler::ast() const {
	if(m_ast.empty()) {
		return std::nullopt;
	}

	return m_ast;
}

/* class Lexer */

Result<std::vector<Token>, AsmError> Lexer::process(const std::string &source) {
	std::vector<Token> tokens;

	u32 lineno = 1;
	for(u32 ix = 0; ix < source.size(); ix++) {
		switch(source[ix]) {
		case ';': /* Comment */
			ix = source.find('\n', ix) + 1;
			lineno++;
			break;
		case '\'':
			tokens.emplace_back(Token::SINGLE_QUOTE, lineno);
			break;
		case '[':
			tokens.emplace_back(Token::LEFT_SQUARE_BRACKET, lineno);
			break;
		case ']':
			tokens.emplace_back(Token::RIGHT_SQUARE_BRACKET, lineno);
			break;
		case '\n':
			lineno++;
			break;
		case '-':
			tokens.emplace_back(Token::MINUS, lineno);
			break;
		case '+':
			tokens.emplace_back(Token::PLUS, lineno);
			break;
		case '/':
			tokens.emplace_back(Token::SLASH, lineno);
			break;
		case '*':
			tokens.emplace_back(Token::TIMES, lineno);
			break;
		case ',':
			tokens.emplace_back(Token::COMMA, lineno);
			break;
		case '\"':
			tokens.emplace_back(Token::DOUBLE_QUOTE, lineno);
			ix += parseStringLiteral(source.substr(ix + 1), lineno, tokens);
			break;
		default: /* Character has no special handling */
			if(std::isspace(source[ix])) {
				continue;
			}

			const usize space_pos = std::distance(
				source.cbegin(), std::find_if(source.cbegin() + ix, source.cend(), [](char c) {
					return std::isspace(c) || isCharReserved(c);
				})); /* yucky */
			const u32 word_len =
				space_pos != source.length() ? space_pos - ix : source.length() - ix;
			std::string word = std::string(source.data() + ix, word_len);

			const Token::Type token_type = Token::typeFromString(word);

			if(token_type == Token::LABEL) {
				word = word.substr(0, word.length() - 1);
				ix++; /* compensate for character lost in word */
			}

			tokens.emplace_back(token_type, lineno, word);
			ix += word.length() - 1;

			break;
		}
	}

	for(auto &tk: tokens) {
		logDebug() << "lexer: " << tk.toString() << "\n";
	}

	logInfo() << "lexed input into " << tokens.size() << " tokens\n";

	return Ok(tokens);
}

u32 Lexer::parseStringLiteral(const std::string &source, u32 &lineno, std::vector<Token> &tokens) {
	if(source.length() == 0) {
		return 0;
	}

	usize ix = 0;
	std::string output;

	for(; ix < source.length(); ix++) {
		if(source[ix] == '\n') {
			lineno++;
			continue;
		}

		if(source[ix] == '"') {
			tokens.emplace_back(Token::STRING, lineno, output);
			tokens.emplace_back(Token::DOUBLE_QUOTE, lineno);
			ix++;
			break;
		}

		if(source[ix] != '\\') {
			output += source[ix];
			continue;
		}

		if(ix + 1 == source.length()) {
			output += '\\';
			continue;
		}

		switch(source[ix + 1]) {
		case 'n':
			output += '\n';
			break;
		case 'r':
			output += '\r';
			break;
		case '"':
			output += '"';
			break;
		case '\\':
			output += '\\';
			break;
		default:
			logWarning() << "line " << lineno << ": unknown escape-sequence \"\\" << source[ix + 1]
						 << "\"\n";
			output += '?';
			break;
		}
		ix++;
	}

	return ix;
}

/* class Parser */

Result<Parser, AsmError> Parser::process(const std::vector<Token> &tokens) {
	Parser parser(tokens);

	const Result<None, AsmError> parse_result = parser.parseTokens();
	if(parse_result.isErr()) {
		return Err(parse_result.unwrapErr());
	}

	return Ok(parser);
}

std::vector<Statement> Parser::ast() const {
	return m_ast;
}

Parser::Parser(const std::vector<Token> &tokens) : m_tokens(tokens) {}

Result<None, AsmError> Parser::parseTokens() {
	for(u32 ix = 0; ix < m_tokens.size(); ix++) {
		const Token &token = m_tokens[ix];

		switch(token.type()) {
		case Token::SECTION: {
			Result<u32, AsmError> parse_result = this->tryParseSectionAt(ix);
			if(parse_result.isErr()) {
				return Err(parse_result.unwrapErr());
			}

			ix = parse_result.unwrap();
			break;
		}
		case Token::LABEL: {
			Result<u32, AsmError> parse_result = this->tryParseLabelAt(ix);
			if(parse_result.isErr()) {
				return Err(parse_result.unwrapErr());
			}

			ix = parse_result.unwrap();
			break;
		}
		case Token::ADDRESSING: {
			Result<u32, AsmError> parse_result = this->tryParseAddressingStatementAt(ix);
			if(parse_result.isErr()) {
				return Err(parse_result.unwrapErr());
			}

			ix = parse_result.unwrap();
			break;
		}
		case Token::UNKNOWN: {
			Result<u32, AsmError> parse_result = this->tryParseUnknownAt(ix);
			if(parse_result.isErr()) {
				return Err(parse_result.unwrapErr());
			}

			ix = parse_result.unwrap();
			break;
		}
		default:
			if(const auto maybe_directive = Directive::kindFromToken(token.type());
			   maybe_directive.has_value()) {
				const Result<u32, AsmError> directive_parse_result =
					this->tryParseDirective(ix + 1, maybe_directive.value());
				if(directive_parse_result.isErr()) {
					return Err(directive_parse_result.unwrapErr());
				}

				ix = directive_parse_result.unwrap();
				continue;
			}

			return Err(AsmError(
				AsmError::SYNTAX_ERROR, token.lineno(),
				"Unexpected keyword or token: " + token.toString()));
		}
	}

	return Ok(None());
}

/* individual statement & expression parsing functions */

Result<u32, AsmError> Parser::tryParseSectionAt(u32 ix) {
	const Token &token = m_tokens[ix];

	if(m_tokens.size() <= ix + 3) {
		return Err(AsmError(AsmError::SYNTAX_ERROR, token.lineno()));
	}

	const Token &literal_name = m_tokens[ix + 1];
	const Token &at_token = m_tokens[ix + 2];
	const Token &literal_value = m_tokens[ix + 3];

	if(literal_name.type() != Token::UNKNOWN) {
		return Err(AsmError(AsmError::SYNTAX_ERROR, token.lineno(), "Expected section name"));
	}
	if(at_token.type() != Token::AT) {
		return Err(AsmError(AsmError::SYNTAX_ERROR, token.lineno(), "Expected keyword \"at\""));
	}
	if(!Token::isNumberType(literal_value.type())) {
		return Err(AsmError(AsmError::SYNTAX_ERROR, token.lineno(), "Expected a number"));
	}

	if(!literal_name.maybeValue().has_value() || !literal_value.maybeValue().has_value()) {
		shared::panic(
			"illegal state: literal_name.maybeValue() || literal_value.maybeValue() is "
			"nullopt");
	}

	/* clang-format off */
	addStatement(Statement(
			Statement::SECTION,
			{
				std::make_shared<Identifier>(
					Identifier::SECTION,
					literal_name.maybeValue().value(),
					literal_name.lineno()
				),
				std::make_shared<Literal>(
					literal_value.toBytes(),
					literal_value.lineno()
				),
			},
			token.lineno()
		)
	);
	/* clang-format on */
	return Ok(ix + 3);
}

Result<u32, AsmError> Parser::tryParseLabelAt(u32 ix) {
	const Token &token = m_tokens[ix];

	if(!token.maybeValue().has_value()) {
		shared::panic(
			"illegal state: token.type() == Token::LABEL && "
			"!token.maybeValue().has_value()");
	}

	/* clang-format off */
	addStatement(Statement(
			Statement::LABEL,
			{
				std::make_shared<Identifier>(
					Identifier::LABEL,
					token.maybeValue().value(),
					token.lineno()
				),
			},
			token.lineno()
		)
	);
	/* clang-format on */
	return Ok(ix);
}

Result<u32, AsmError> Parser::tryParseAddressingStatementAt(u32 ix) {
	const Token &token = m_tokens[ix];

	if(ix + 1 >= m_tokens.size()) {
		return Err(AsmError(
			AsmError::SYNTAX_ERROR, token.lineno(),
			"Expected keyword \"relative\" or \"absolute\" after \"addressing\""));
	}

	const Token &addressing_type_token = m_tokens[ix + 1];
	if(addressing_type_token.type() != Token::ABSOLUTE &&
	   addressing_type_token.type() != Token::RELATIVE) {
		return Err(AsmError(
			AsmError::SYNTAX_ERROR, token.lineno(),
			"Expected keyword \"relative\" or \"absolute\" after \"addressing\""));
	}

	const bool relative = addressing_type_token.type() == Token::RELATIVE;

	/* clang-format off */
	addStatement(Statement(
			relative ? Statement::ADDRESSING_RELATIVE : Statement::ADDRESSING_ABSOLUTE,
			{}, token.lineno()
		)
	);

	logDebug() << "added addressing (relative|absolute) statement\n";
	/* clang-format on */
	return Ok(ix + 1);
}

Result<u32, AsmError> Parser::tryParseUnknownAt(u32 ix) {
	const Token &token = m_tokens[ix];

	if(!token.maybeValue().has_value()) {
		shared::panic(
			"illegal state: token.type() == Token::UNKNOWN && "
			"!token.maybeValue().has_value()");
	}

	const std::string token_value = token.maybeValue().value();

	if(const auto maybe_instruction = Instruction::kindFromString(token_value);
	   maybe_instruction.has_value()) {
		return this->tryParseInstruction(ix + 1, maybe_instruction.value());
	}

	return Err(AsmError(AsmError::SYNTAX_ERROR, token.lineno()));
}

Result<u32, AsmError> Parser::tryParseInstruction(u32 ix, Instruction::Kind kind) {
	const Token &token = m_tokens[ix];
	const std::vector<InstructionOperand> required_operands = InstructionOperand::operandsFor(kind);
	const Result<std::pair<u32, std::vector<std::shared_ptr<ExpressionBase>>>, AsmError>
		parse_operands_result = this->tryParseOperands(ix);

	if(parse_operands_result.isErr()) {
		return Err(parse_operands_result.unwrapErr());
	}

	const std::pair<u32, std::vector<std::shared_ptr<ExpressionBase>>> &parse_operands_unwrapped =
		parse_operands_result.unwrap();

	const std::vector<std::shared_ptr<ExpressionBase>> &operand_expressions =
		parse_operands_unwrapped.second;

	if(operand_expressions.size() != required_operands.size()) {
		return Err(AsmError(
			AsmError::ILLEGAL_OPERAND, m_tokens[ix].lineno(),
			"Expected " + std::to_string(required_operands.size()) + " operands, got " +
				std::to_string(operand_expressions.size())));
	}

	const auto given_operands =
		map(operand_expressions,
			[](std::shared_ptr<ExpressionBase> expr) -> InstructionOperand::Kind {
				switch(expr->kind()) {
				case ExpressionBase::DIRECT_ADDRESS: {
					const DirectAddress *tmp_directaddress =
						dynamic_cast<DirectAddress *>(expr.get());
					if(tmp_directaddress == nullptr) {
						shared::panic("could not cast ExpressionBase to DirectAddress in map func");
					}
					return tmp_directaddress->kind() == DirectAddress::MEMORY
							   ? InstructionOperand::DIRECT
							   : InstructionOperand::REGISTER_DIRECT;
				}
				case ExpressionBase::INDIRECT_ADDRESS: {
					const IndirectAddress *tmp_indirectaddress =
						dynamic_cast<IndirectAddress *>(expr.get());
					if(tmp_indirectaddress == nullptr) {
						shared::panic(
							"could not cast ExpressionBase to IndirectAddress in map func");
					}
					return tmp_indirectaddress->kind() == IndirectAddress::MEMORY
							   ? InstructionOperand::DIRECT
							   : InstructionOperand::REGISTER_DIRECT;
				}
				case ExpressionBase::REGISTER:
					return InstructionOperand::REGISTER_IMMEDIATE;
				case ExpressionBase::IDENTIFIER:
				case ExpressionBase::LITERAL:
					return InstructionOperand::IMMEDIATE;
				case ExpressionBase::STATEMENT_EXPRESSION:
					shared::panic("invalid state: tryParseOperands returned a StatementExpression");
				}
			})
			.to<std::vector<InstructionOperand::Kind>>();

	for(u32 operand_ix = 0; operand_ix < required_operands.size(); operand_ix++) {
		if(required_operands[operand_ix].isAllowed(given_operands[operand_ix])) {
			continue;
		}

		logDebug() << "operand_ix: " << operand_ix << "\n";

		return Err(AsmError(
			AsmError::ILLEGAL_OPERAND, m_tokens[ix].lineno(),
			"Invalid operand of type " + std::to_string(given_operands[operand_ix])));
	}

	/* clang-format off */
	addStatement(Statement(
			Statement::INSTRUCTION,
			{},
			token.lineno(),
			std::make_shared<Instruction>(
				kind,
				operand_expressions,
				token.lineno()
			)
		)
	);
	/* clang-format on */
	return Ok(parse_operands_unwrapped.first);
}

Result<u32, AsmError> Parser::tryParseDirective(u32 ix, Directive::Kind kind) {
	const Token &token = m_tokens[ix];
	const std::vector<DirectiveOperand> required_operands = DirectiveOperand::operandsFor(kind);
	const Result<std::pair<u32, std::vector<std::shared_ptr<ExpressionBase>>>, AsmError>
		parse_operands_result = this->tryParseOperands(ix);

	if(parse_operands_result.isErr()) {
		return Err(parse_operands_result.unwrapErr());
	}

	const std::pair<u32, std::vector<std::shared_ptr<ExpressionBase>>> &parse_operands_unwrapped =
		parse_operands_result.unwrap();

	const std::vector<std::shared_ptr<ExpressionBase>> &operand_expressions =
		parse_operands_unwrapped.second;

	if(operand_expressions.size() != required_operands.size()) {
		return Err(AsmError(
			AsmError::ILLEGAL_OPERAND, m_tokens[ix].lineno(),
			"Expected " + std::to_string(required_operands.size()) + " operands, got " +
				std::to_string(operand_expressions.size())));
	}

	const auto given_operands =
		map(operand_expressions,
			[](std::shared_ptr<ExpressionBase> expr) -> DirectiveOperand::Kind {
				switch(expr->kind()) {
				case ExpressionBase::DIRECT_ADDRESS:
				case ExpressionBase::INDIRECT_ADDRESS:
				case ExpressionBase::REGISTER:
					return DirectiveOperand::INVALID;
				case ExpressionBase::IDENTIFIER:
				case ExpressionBase::LITERAL:
					return DirectiveOperand::IMMEDIATE;
				case ExpressionBase::STATEMENT_EXPRESSION:
					shared::panic("invalid state: tryParseOperands returned a StatementExpression");
				}
			})
			.to<std::vector<DirectiveOperand::Kind>>();

	for(u32 operand_ix = 0; operand_ix < required_operands.size(); operand_ix++) {
		if(required_operands[operand_ix].isAllowed(given_operands[operand_ix])) {
			continue;
		}

		logDebug() << "operand_ix: " << operand_ix << "\n";

		return Err(AsmError(
			AsmError::ILLEGAL_OPERAND, m_tokens[ix].lineno(),
			"Invalid operand of type " + std::to_string(given_operands[operand_ix])));
	}

	auto new_directive = std::make_shared<Directive>(kind, operand_expressions, token.lineno());

	/* clang-format off */
	addStatement(Statement(
			Statement::DIRECTIVE,
			{},
			token.lineno(),
			new_directive
		)
	);
	/* clang-format on */

	if(kind == Directive::TIMES) {
		m_timesDirectiveDeclared = true;
		m_lastTimesDirective = new_directive;
	}
	return Ok(parse_operands_unwrapped.first);
}

Result<std::pair<u32, std::vector<std::shared_ptr<ExpressionBase>>>, AsmError>
Parser::tryParseOperands(u32 ix) {
	if(ix >= m_tokens.size()) {
		return Ok(
			std::pair<u32, std::vector<std::shared_ptr<ExpressionBase>>>(
				ix, std::vector<std::shared_ptr<ExpressionBase>>{}));
	}

	const u32 lineno = m_tokens[ix - 1].lineno();
	logDebug() << "parsing operands for token on line: " << lineno << "\n";

	std::vector<std::shared_ptr<ExpressionBase>> expressions;

	/* Parse the following expressions:
	 * 1. Identifiers
	 * 2. Literals
	 * 3. Registers
	 * 4. Direct Addressing ([<expression>])
	 * 5. Indirect Addressing ([[<expression>]])
	 */

	ix--; /* this is shitty */

	do {
		ix++;
		logDebug() << "ix = " << ix << "; current token = " << m_tokens[ix].toString() << "\n";

		const Token &token = m_tokens[ix];
		if(token.lineno() != lineno) {
			break;
		}

		if(Token::isNumberType(token.type()) || token.type() == Token::STRING) {
			expressions.push_back(std::make_shared<Literal>(token.toBytes(), token.lineno()));
			goto end;
		}

		if(token.type() == Token::DOUBLE_QUOTE) {
			if(ix + 2 >= m_tokens.size() || m_tokens[ix + 2].type() != Token::DOUBLE_QUOTE) {
				return Err(AsmError(AsmError::SYNTAX_ERROR, token.lineno(), "unclosed string"));
			}

			expressions.push_back(
				std::make_shared<Literal>(m_tokens[ix + 1].toBytes(), m_tokens[ix + 1].lineno()));

			ix += 2;
			goto end;
		}

		if(Token::isRegister(token.type())) {
			const std::optional<Register> reg = Register::fromToken(token);
			if(!reg.has_value()) {
				shared::panic(
					"invalid return: token = " + token.toString() +
					"; Token::isRegister() == true; Register::fromToken() returned "
					"nullopt!");
			}
			expressions.push_back(std::make_shared<Register>(reg.value()));
			goto end;
		}

		/* direct/indirect addressing */
		if(token.type() == Token::LEFT_SQUARE_BRACKET) {
			if(ix + 2 >= m_tokens.size()) {
				return Err(AsmError(
					AsmError::SYNTAX_ERROR, token.lineno(),
					"invalid (in)direct addressing syntax"));
			}

			/* boiled down rules here are:
			 * A right closing bracket is required. If there is one opening, one closing is
			 * required, if there are two opening, two closers are required. The middle part may
			 * be a literal, identifier or register.
			 *
			 * See chapter 2.4.1. of the MFDASM spec in the root of the repository.
			 */

			const bool indirect =
				ix + 1 < m_tokens.size() && m_tokens[ix + 1].type() == Token::LEFT_SQUARE_BRACKET;
			const u32 offset = indirect ? 2 : 1;

			bool syntax_valid = false;
			if(indirect) {
				syntax_valid = ix + 4 < m_tokens.size() &&
							   m_tokens[ix + 3].type() == Token::RIGHT_SQUARE_BRACKET &&
							   m_tokens[ix + 4].type() == Token::RIGHT_SQUARE_BRACKET;
			} else {
				syntax_valid = m_tokens[ix + 2].type() == Token::RIGHT_SQUARE_BRACKET;
			}

			if(!syntax_valid) {
				return Err(AsmError(
					AsmError::SYNTAX_ERROR, token.lineno(),
					std::string("invalid ") + (indirect ? "in" : "") + "direct addressing syntax"));
			}

			logDebug() << "direct? " << (indirect ? "no" : "yes")
					   << "; token: " << m_tokens[ix + offset].toString() << "\n";

			const Token &middle_token = m_tokens[ix + offset];
			logDebug() << "recurse try parse operands, ix " << ix << "\n";
			Result<std::pair<u32, std::vector<std::shared_ptr<ExpressionBase>>>, AsmError>
				maybe_expr = this->tryParseOperands(ix + offset);
			if(maybe_expr.isErr()) {
				return maybe_expr;
			}

			const std::shared_ptr<ExpressionBase> expr = maybe_expr.unwrap().second[0];
			ix = maybe_expr.unwrap().first;

			if(indirect) {
				expressions.push_back(
					std::make_shared<IndirectAddress>(
						Token::isRegister(middle_token.type()) ? IndirectAddress::REGISTER
															   : IndirectAddress::MEMORY,
						expr, middle_token.lineno()));
			} else {
				expressions.push_back(
					std::make_shared<DirectAddress>(
						Token::isRegister(middle_token.type()) ? DirectAddress::REGISTER
															   : DirectAddress::MEMORY,
						expr, middle_token.lineno()));
			}

			ix += indirect ? 2 : 1;
			goto end;
		}

		if(token.type() == Token::_HERE) {
			expressions.push_back(
				std::make_shared<Identifier>(Identifier::HERE, "", token.lineno()));
			goto end;
		}

		if(token.type() != Token::UNKNOWN) {
			break;
		}

		expressions.push_back(
			std::make_shared<Identifier>(
				Identifier::LABEL, token.maybeValue().value_or("???"), token.lineno()));

	end:
		ix++;
	} while(ix < m_tokens.size() && m_tokens[ix].type() == Token::COMMA);

	ix--; /* ix needs to be on the last "consumed" token */
	return Ok(std::pair<u32, std::vector<std::shared_ptr<ExpressionBase>>>(ix, expressions));
}

void Parser::addStatement(Statement statement) {
	if(!m_timesDirectiveDeclared) {
		m_ast.push_back(std::move(statement));
		return;
	}

	m_timesDirectiveDeclared = false;
	if(m_lastTimesDirective == nullptr) {
		shared::panic("m_timesDirectiveDeclared true but m_lastTimesDirective == nullptr");
	}

	m_lastTimesDirective->addExpression(
		std::make_shared<StatementExpression>(statement, statement.lineno()));
}

}  // namespace mfdasm::impl
