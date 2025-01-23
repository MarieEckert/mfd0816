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

#include <cxxabi.h>
#include <iostream>
#include <memory>

#include <execinfo.h>

#include <mfdemu/panic.hpp>

namespace mfdemu {

static std::string demangle(const char *const symbol) {
	const std::unique_ptr<char, decltype(&std::free)> demangled(
		abi::__cxa_demangle(symbol, 0, 0, 0), &std::free);
	if(demangled) {
		return demangled.get();
	} else {
		return symbol;
	}
}

static void backtrace() {
	void *addresses[256];
	const int n = ::backtrace(addresses, std::extent<decltype(addresses)>::value);
	const std::unique_ptr<char *, decltype(&std::free)> symbols(
		::backtrace_symbols(addresses, n), &std::free);

	for(int i = 0; i < n; ++i) {
		/* we parse the symbols retrieved from backtrace_symbols() to
		 * extract the "real" symbols that represent the mangled names.
		 */
		char *const symbol = symbols.get()[i];
		char *end = symbol;
		while(*end) {
			++end;
		}

		/* scanning is done backwards, since the module name
		 * might contain both '+' or '(' characters.
		 */
		while(end != symbol && *end != '+') {
			--end;
		}

		char *begin = end;
		while(begin != symbol && *begin != '(') {
			--begin;
		}

		if(begin != symbol) {
			std::cerr << (i == 2 ? "  -> " : "     ") << i << ": "
					  << std::string(symbol, ++begin - symbol);
			*end++ = '\0';
			std::cerr << demangle(begin) << '+' << end;
		} else {
			std::cerr << symbol;
		}
		std::cerr << std::endl;
	}
}

[[noreturn]] void panic(const std::string &error) {
	std::cerr << "mfdemu panic'd: " << error << "\n";
	std::cerr << "backtrace:\n";
	backtrace();
	std::exit(100);
}
}  // namespace mfdemu