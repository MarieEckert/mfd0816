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
		case '\"':
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
				continue;
			case '\'': /* String opening */
				continue;
			case '[':
				this->m_tokens.emplace_back(Token::Type::LEFT_SQUARE_BRACKET, lineno);
				break;
			case ']':
				this->m_tokens.emplace_back(Token::Type::RIGHT_SQUARE_BRACKET, lineno);
				break;
			case '\n':
				lineno++;
				break;
			case '-':
				this->m_tokens.emplace_back(Token::Type::MINUS, lineno);
				break;
			case '+':
				this->m_tokens.emplace_back(Token::Type::PLUS, lineno);
				break;
			case '/':
				this->m_tokens.emplace_back(Token::Type::SLASH, lineno);
				break;
			case '*':
				this->m_tokens.emplace_back(Token::Type::TIMES, lineno);
				break;
			case ',':
				this->m_tokens.emplace_back(Token::Type::COMMA, lineno);
				break;
			default: /* Character has no special handling */
				if(std::isspace(source[ix])) {
					continue;
				}

				u32 consumed_lines = 0;

				const usize space_pos = std::distance(
					source.cbegin(),
					std::find_if(source.cbegin() + ix, source.cend(), [&consumed_lines](char c) {
						if(c == '\n') {
							consumed_lines++;
						}
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
				lineno += consumed_lines;

				break;
		}
	}

	for(auto &tk: this->m_tokens) {
		std::cout << tk.toString() << "\n";
	}

	return Ok(None());
}

}  // namespace sasm::impl