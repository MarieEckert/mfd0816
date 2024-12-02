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

#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

#include <mfdasm/impl/ast.hpp>
#include <mfdasm/panic.hpp>
#include <mfdasm/typedefs.hpp>

namespace mfdasm::impl {
/* class Statement */

Statement::Statement(
	Kind kind,
	std::vector<ExpressionBase> expression,
	std::unique_ptr<StatementBase> statement) {
	if(statement == nullptr && (kind == Kind::INSTRUCTION || kind == Kind::DIRECTIVE)) {
		panic("bad software design (kind = " + std::to_string(kind) + "; statement = nullptr)");
	}
}

std::string Statement::toString() const {
	return "";
}

std::vector<u8> Statement::toBytes(ResovalContext &resolval_context) const {
	return {};
}

}  // namespace mfdasm::impl