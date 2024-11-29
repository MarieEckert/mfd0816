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
#include <impl/assembler.hpp>
#include <iterator>

namespace sasm::impl {

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
		std::cout << tk.toString() << "\n";
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

}  // namespace sasm::impl