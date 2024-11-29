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

#ifndef MFDASM_IMPL_SECTION_HPP
#define MFDASM_IMPL_SECTION_HPP

#include <vector>

#include <3rdparty/result.hpp>

#include <mfdasm/impl/asmerror.hpp>
#include <mfdasm/impl/token.hpp>
#include <mfdasm/typedefs.hpp>

namespace mfdasm::impl {

class Section {
   public:
	Section(const std::vector<Token> &tokens);

	Result<std::vector<u8>, AsmError> assemble();

   private:
	usize m_srcLine;

	u16 m_sectionLocation;

	std::vector<Token> m_tokens;
};

}  // namespace mfdasm::impl

#endif