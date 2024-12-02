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

#include <fstream>
#include <iostream>
#include <ostream>

#include <3rdparty/ansi.h>

#include <mfdasm/log.hpp>

namespace mfdasm {

static Logger::Level _log_level = Logger::Level::DEBUG;

void Logger::setLogLevel(Level level) {
	_log_level = level;
}

std::ostream &Logger::getStream(Level level) {
	static std::ofstream void_stream("/dev/null");
	if(level < _log_level) {
		return void_stream;
	}

	switch(level) {
		case Level::DEBUG:
			std::cerr << ANSI_FG_BBLACK "DEBUG:   " ANSI_RESET;
			break;
		case Level::INFO:
			std::cerr << "INFO:    ";
			break;
		case Level::WARNING:
			std::cerr << ANSI_FG_YELLOW "WARNING: " ANSI_RESET;
			break;
		case Level::ERROR:
			std::cerr << ANSI_FG_RED "ERROR:   " ANSI_RESET;
			break;
	}

	return std::cerr;
}

}  // namespace mfdasm