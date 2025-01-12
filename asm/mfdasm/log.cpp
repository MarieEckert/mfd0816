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

#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <unordered_map>

#include <3rdparty/ansi.h>

#include <mfdasm/log.hpp>
#include <mfdasm/panic.hpp>

namespace mfdasm {

static Logger::Level _log_level =
#ifndef MFDASM_RELEASE
	Logger::Level::DEBUG
#else
	Logger::Level::INFO
#endif
	;

void Logger::setLogLevel(Level level) {
	if(level > Level::PANIC) {
		panic("invalid log level: " + std::to_string(static_cast<u8>(level)));
	}
	_log_level = level;
}

void Logger::stringSetLogLevel(const std::string &level) {
	if(level.empty()) {
		return;
	}

	static const std::unordered_map<std::string, Level> levels = {
		{"debug", Level::DEBUG}, {"info", Level::INFO},	  {"warn", Level::WARNING},
		{"error", Level::ERROR}, {"panic", Level::PANIC},
	};

	auto iter = levels.find(level);

	if(iter == levels.cend()) {
		panic("Invalid log level \"" + level + "\"\n");
	}

	setLogLevel(iter->second);
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
	case Level::PANIC: /* this is more of a "symbolic" level, since you cant disable logging of
						* panic messages
						*/
		return void_stream;
	}

	return std::cerr;
}

}  // namespace mfdasm
