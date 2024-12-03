/* This file is part of MFDASM.
 *
 * MFDASM is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * MFDASM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * MFDASM. If not, see <https://www.gnu.org/licenses/>.
 */

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <iterator>

#include <mfdasm/impl/assembler.hpp>
#include <mfdasm/impl/ast.hpp>
#include <mfdasm/log.hpp>

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

	return Ok(None());
}

Result<None, AsmError> Assembler::lexInput(const std::string &source) {
	u32 lineno = 0;
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
		const Token token = this->m_tokens[ix];

		switch(token.type()) {
			case Token::SECTION: {
				if(this->m_tokens.size() <= ix + 3) {
					return Err(AsmError::SYNTAX_ERROR);
				}

				const Token literal_name = this->m_tokens[ix + 1];
				const Token at_token = this->m_tokens[ix + 2];
				const Token literal_value = this->m_tokens[ix + 3];

				if(literal_name.type() != Token::UNKNOWN || at_token.type() != Token::AT ||
				   !Token::isNumberType(literal_value.type())) {
					return Err(AsmError::SYNTAX_ERROR);
				}

				if(!literal_name.maybeValue().has_value() ||
				   !literal_value.maybeValue().has_value()) {
					return Err(AsmError::SYNTAX_ERROR);
				}

				const u16 value = std::stoi(
					literal_value.maybeValue().value(), 0,
					Token::numberTypeBase(literal_value.type()));

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
				break;
			}
			default:
				logWarning() << "Unhandled token " << token.type() << "\n";
				break;
		}
	}
	return Ok(None());
}

}  // namespace mfdasm::impl