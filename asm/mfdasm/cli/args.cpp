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

#include "args.hpp"

namespace mfdasm::cli {

/* class ArgumentBase */

ArgumentBase::ArgumentBase(std::string short_name, std::string long_name, bool action)
	: m_isAction(action), m_shortName(short_name), m_longName(long_name) {}

ssize_t ArgumentBase::check(const std::string &name, std::string value) {
	if(name != m_shortName && name != m_longName) {
		return -1;
	}

	m_stringValue = !m_isAction ? value : "true";
	return m_isAction ? 0 : 1;
}

/* class ArgumentParser */

void ArgumentParser::parse(int argc, char **argv) {
	std::vector<std::string> str_args;
	str_args.reserve(argc);

	for(int ix = 0; ix < argc; ix++) {
		str_args.emplace_back(argv[ix]);
	}

	for(size_t ix = 0; ix < str_args.size(); ix++) {
		for(const auto &arg: this->args) {
			ssize_t inc =
				arg->check(str_args[ix], ix + 1 < str_args.size() ? str_args[ix + 1] : "");
			;
			if(inc == -1) {
				continue;
			}

			ix += inc;
			break;
		}
	}
}

}  // namespace mfdasm::cli