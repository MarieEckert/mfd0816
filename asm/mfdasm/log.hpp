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

#ifndef MFDASM_LOG_HPP
#define MFDASM_LOG_HPP

#include <ostream>

#include <mfdasm/typedefs.hpp>

/* clang-format off */
#define logDebug()   mfdasm::Logger::getStream(mfdasm::Logger::Level::DEBUG)
#define logInfo()    mfdasm::Logger::getStream(mfdasm::Logger::Level::INFO)
#define logWarning() mfdasm::Logger::getStream(mfdasm::Logger::Level::WARNING)
#define logError()   mfdasm::Logger::getStream(mfdasm::Logger::Level::ERROR)
/* clang-format on */

namespace mfdasm {

class Logger {
   public:
	enum class Level : u8 {
		DEBUG,
		INFO,
		WARNING,
		ERROR,
	};

	static void setLogLevel(Level level);
	static std::ostream &getStream(Level level);
};

}  // namespace mfdasm

#endif