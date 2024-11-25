/* This file is part of MFDASM.
 *
 * MFDASM is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * MFDASM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * MFDASM. If not, see <https://www.gnu.org/licenses/>.
 */

#include <cctype>
#include <impl/assembler.hpp>

namespace sasm::impl {

Result<None, AsmError> Assembler::parseLines(const std::string &source) {
	if(source.empty()) {
		return Ok(None());
	}

	u32 lineno = 0;
	for(u32 ix = 0; ix < source.size(); ix++) {
		switch(source[ix]) {
			case ';':
				ix = source.find("\n", ix) + 1;
				lineno++;
				continue;
			default:
				if(std::isspace(source[ix])) {
					continue;
				}

				const usize space_pos = source.find(" ", ix);
				const u32 word_len =
					space_pos != std::string::npos ? space_pos - ix : source.length() - ix;
				const std::string word = std::string(source.data() + ix, word_len);
				const Token::Type token = Token::typeFromString(word);
				this->m_tokens.emplace_back(token, lineno);
				ix += word.length();
				std::cout << "added token " << token << " for word " << word << "\n";

				break;
		}
	}

	return Ok(None());
}

}  // namespace sasm::impl