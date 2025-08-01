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

#ifndef MFDASM_LOG_HPP
#define MFDASM_LOG_HPP

#include <ostream>

#include "typedefs.hpp"

/* clang-format off */
#define logDebug()   shared::Logger::getStream(shared::Logger::DEBUG)
#define logInfo()    shared::Logger::getStream(shared::Logger::INFO)
#define logWarning() shared::Logger::getStream(shared::Logger::WARNING)
#define logError()   shared::Logger::getStream(shared::Logger::ERROR)
/* clang-format on */

namespace shared {

class Logger {
   public:
	enum Level : u8 {
		DEBUG,
		INFO,
		WARNING,
		ERROR,
		PANIC,
	};

	static void setLogLevel(Level level);
	static void stringSetLogLevel(const std::string &level);
	static std::ostream &getStream(Level level);
};

}  // namespace shared

#endif
