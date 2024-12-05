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
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include <mfdasm/impl/assembler.hpp>
#include <mfdasm/impl/ast.hpp>
#include <mfdasm/impl/instruction_operand.hpp>
#include <mfdasm/log.hpp>
#include <mfdasm/panic.hpp>

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

Result<None, AsmError> Assembler::parseLines(const std::string &source) {
	if(source.empty()) {
		return Ok(None());
	}

	Result<None, AsmError> lex_result = this->lexInput(source);
	if(lex_result.isErr()) {
		return Err(lex_result.unwrapErr());
	}

	Result<None, AsmError> parse_result = this->parseTokens();
	if(parse_result.isErr()) {
		return Err(parse_result.unwrapErr());
	}

	return Ok(None());
}

Result<None, AsmError> Assembler::lexInput(const std::string &source) {
	u32 lineno = 1;
	for(u32 ix = 0; ix < source.size(); ix++) {
		switch(source[ix]) {
			case ';': /* Comment */
				ix = source.find('\n', ix) + 1;
				lineno++;
				break;
			case '\'':
				this->m_tokens.emplace_back(Token::SINGLE_QUOTE, lineno);
				break;
			case '[':
				this->m_tokens.emplace_back(Token::LEFT_SQUARE_BRACKET, lineno);
				break;
			case ']':
				this->m_tokens.emplace_back(Token::RIGHT_SQUARE_BRACKET, lineno);
				break;
			case '\n':
				lineno++;
				break;
			case '-':
				this->m_tokens.emplace_back(Token::MINUS, lineno);
				break;
			case '+':
				this->m_tokens.emplace_back(Token::PLUS, lineno);
				break;
			case '/':
				this->m_tokens.emplace_back(Token::SLASH, lineno);
				break;
			case '*':
				this->m_tokens.emplace_back(Token::TIMES, lineno);
				break;
			case ',':
				this->m_tokens.emplace_back(Token::COMMA, lineno);
				break;
			case '\"':
				this->m_tokens.emplace_back(Token::DOUBLE_QUOTE, lineno);
				ix += this->parseStringLiteral(source.substr(ix + 1), lineno);
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

				this->m_tokens.emplace_back(token_type, lineno, word);
				ix += word.length() - 1;

				break;
		}
	}

	for(auto &tk: this->m_tokens) {
		logDebug() << "lexer: " << tk.toString() << "\n";
	}

	logInfo() << "lexed input into " << this->m_tokens.size() << " tokens\n";

	return Ok(None());
}

u32 Assembler::parseStringLiteral(const std::string &source, u32 &lineno) {
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
			this->m_tokens.emplace_back(Token::STRING, lineno, output);
			this->m_tokens.emplace_back(Token::DOUBLE_QUOTE, lineno);
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
			case '"':
				output += '"';
				break;
			case '\\':
				output += '\\';
				break;
			default:
				output += '?';
				break;
		}
		ix++;
	}

	return ix;
}

Result<None, AsmError> Assembler::parseTokens() {
	for(u32 ix = 0; ix < this->m_tokens.size(); ix++) {
		const Token &token = this->m_tokens[ix];

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
				logInfo()
					<< "TODO: implement handling for unknown tokens (instructions or directives)\n";
				break;
			}
			default:
				return Err(AsmError(
					AsmError::SYNTAX_ERROR, token.lineno(),
					"Unexpected keyword or token: " + token.toString()));
		}
	}

	return Ok(None());
}

Result<u32, AsmError> Assembler::tryParseSectionAt(u32 ix) {
	const Token &token = this->m_tokens[ix];

	if(this->m_tokens.size() <= ix + 3) {
		return Err(AsmError(AsmError::SYNTAX_ERROR, token.lineno()));
	}

	const Token &literal_name = this->m_tokens[ix + 1];
	const Token &at_token = this->m_tokens[ix + 2];
	const Token &literal_value = this->m_tokens[ix + 3];

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
		panic(
			"illegal state: literal_name.maybeValue() || literal_value.maybeValue() is "
			"nullopt");
	}

	const u16 value = std::stoi(
		literal_value.maybeValue().value(), 0, Token::numberTypeBase(literal_value.type()));

	/* clang-format off */
	this->m_ast.push_back(Statement(
			Statement::SECTION,
			{
				Identifier(
					Identifier::SECTION,
					literal_name.maybeValue().value()
				),
				Literal(
					{
						static_cast<u8>((value >> 8) & 0xFF),
						static_cast<u8>(value & 0xFF)
					}
				)
			}
		)
	);
	/* clang-format on */
	return Ok(ix + 3);
}

Result<u32, AsmError> Assembler::tryParseLabelAt(u32 ix) {
	const Token &token = this->m_tokens[ix];

	if(!token.maybeValue().has_value()) {
		panic(
			"illegal state: token.type() == Token::LABEL && "
			"!token.maybeValue().has_value()");
	}

	/* clang-format off */
	this->m_ast.push_back(Statement(
			Statement::LABEL,
			{
				Identifier(
					Identifier::LABEL,
					token.maybeValue().value()
				)
			}
		)
	);
	/* clang-format on */
	return Ok(ix);
}

Result<u32, AsmError> Assembler::tryParseAddressingStatementAt(u32 ix) {
	const Token &token = this->m_tokens[ix];

	if(ix + 1 >= this->m_tokens.size()) {
		return Err(AsmError(
			AsmError::SYNTAX_ERROR, token.lineno(),
			"Expected keyword \"relative\" or \"absolute\" after \"addressing\""));
	}

	const Token &addressing_type_token = this->m_tokens[ix + 1];
	if(addressing_type_token.type() != Token::ABSOLUTE &&
	   addressing_type_token.type() != Token::RELATIVE) {
		return Err(AsmError(
			AsmError::SYNTAX_ERROR, token.lineno(),
			"Expected keyword \"relative\" or \"absolute\" after \"addressing\""));
	}

	const bool relative = addressing_type_token.type() == Token::RELATIVE;

	/* clang-format off */
	this->m_ast.push_back(Statement(
			relative ? Statement::ADDRESSING_RELATIVE : Statement::ADDRESSING_ABSOLUTE,
			{}
		)
	);

	logDebug() << "added addressing (relative|absolute) statement\n";
	/* clang-format on */
	return Ok(ix + 1);
}

Result<u32, AsmError> Assembler::tryParseUnknownAt(u32 ix) {
	const Token &token = this->m_tokens[ix];

	if(!token.maybeValue().has_value()) {
		panic(
			"illegal state: token.type() == Token::UNKNOWN && "
			"!token.maybeValue().has_value()");
	}

	const std::string token_value = token.maybeValue().value();

	if(const auto maybe_instruction = Instruction::kindFromString(token_value);
	   maybe_instruction.has_value()) {
		return this->tryParseInstruction(ix + 1, maybe_instruction.value());
	}

	if(const auto maybe_directive = Directive::kindFromString(token_value)) {
		return this->tryParseDirective(ix + 1, maybe_directive.value());
	}

	return Err(AsmError(AsmError::SYNTAX_ERROR, token.lineno()));
}

Result<u32, AsmError> Assembler::tryParseInstruction(u32 ix, Instruction::Kind kind) {
	const std::vector<InstructionOperand> required_operands = InstructionOperand::operandsFor(kind);
	const Result<std::pair<u32, std::vector<ExpressionBase>>, AsmError> parse_operands_result =
		this->tryParseOperands(ix);

	if(parse_operands_result.isErr()) {
		return Err(parse_operands_result.unwrapErr());
	}

	const std::pair<u32, std::vector<ExpressionBase>> &parse_operands_unwrapped =
		parse_operands_result.unwrap();

	const std::vector<ExpressionBase> &operand_expressions = parse_operands_unwrapped.second;

	if(operand_expressions.size() != required_operands.size()) {
		return Err(AsmError(
			AsmError::ILLEGAL_OPERAND, this->m_tokens[ix].lineno(),
			"Expected " + std::to_string(required_operands.size()) + " operands, got " +
				std::to_string(operand_expressions.size())));
	}

	const auto given_operands =
		map(operand_expressions, [](ExpressionBase expr) -> InstructionOperand::Kind {
			switch(expr.kind()) {
				case ExpressionBase::DIRECT_ADDRESS: {
					const DirectAddress &tmp_directaddress = static_cast<DirectAddress &>(expr);
					return tmp_directaddress.kind() == DirectAddress::MEMORY
							   ? InstructionOperand::DIRECT
							   : InstructionOperand::REGISTER_DIRECT;
				}
				case ExpressionBase::INDIRECT_ADDRESS: {
					const IndirectAddress &tmp_indirectaddress =
						static_cast<IndirectAddress &>(expr);
					return tmp_indirectaddress.kind() == IndirectAddress::MEMORY
							   ? InstructionOperand::DIRECT
							   : InstructionOperand::REGISTER_DIRECT;
				}
				case ExpressionBase::REGISTER:
					return InstructionOperand::REGISTER_IMMEDIATE;
				case ExpressionBase::IDENTIFIER:
				case ExpressionBase::LITERAL:
					return InstructionOperand::IMMEDIATE;
			}
		}).to<std::vector<InstructionOperand::Kind>>();

	for(u32 operand_ix = 0; operand_ix < required_operands.size(); operand_ix++) {
		if(required_operands[operand_ix].isAllowed(given_operands[operand_ix])) {
			continue;
		}

		return Err(AsmError(
			AsmError::ILLEGAL_OPERAND, this->m_tokens[ix].lineno(),
			"Invalid operand of type " + std::to_string(given_operands[operand_ix])));
	}

	/* clang-format off */
	this->m_ast.push_back(Statement(
			Statement::INSTRUCTION,
			{},
			std::make_unique<Instruction>(
				kind,
				operand_expressions
			)
		)
	);
	/* clang-format on */
	return Ok(parse_operands_unwrapped.first);
}

Result<u32, AsmError> Assembler::tryParseDirective(u32 ix, Directive::Kind kind) {
	/** @todo implement */
	return Ok(ix);
}

Result<std::pair<u32, std::vector<ExpressionBase>>, AsmError> Assembler::tryParseOperands(u32 ix) {
	if(ix >= this->m_tokens.size()) {
		return Ok(std::pair<u32, std::vector<ExpressionBase>>(ix, std::vector<ExpressionBase>{}));
	}

	std::vector<ExpressionBase> expressions;

	/**
	 * @todo Parse the following expressions:
	 * 1. Identifiers
	 * 2. Literals
	 * 3. Registers
	 * 4. Direct Addressing ([<expression>])
	 * 5. Indirect Addressing ([[<expression>]])
	 */

	return Ok(std::pair<u32, std::vector<ExpressionBase>>(ix, expressions));
}

}  // namespace mfdasm::impl
