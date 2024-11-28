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

#ifndef SASM_IMPL_ASSEMBLER_HPP
#define SASM_IMPL_ASSEMBLER_HPP

#include <impl/asmerror.hpp>
#include <impl/section.hpp>
#include <impl/token.hpp>
#include <result.hpp>
#include <vector>

namespace sasm::impl {

class Assembler {
   public:
	Result<None, AsmError> parseLines(const std::string &source);

   private:
	u32 parseStringLiteral(const std::string &source);

	std::vector<Token> m_tokens;

	std::vector<Section> m_sections;
};

}  // namespace sasm::impl

#endif