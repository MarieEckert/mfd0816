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

#define VERSION "0.0 (develop)"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <shared/cli/args.hpp>
#include <shared/log.hpp>

[[noreturn]] static void licenses() {
	std::cerr << "MFDASM "
				 "---------------------------------------------------------------"
				 "----------\n\n"
			  << "Copyright (C) 2024  Marie Eckert\n"
			  << "Licensed under the GPL v3 License.\n"
			  << "See <https://www.gnu.org/licenses/>.\n";
	std::cerr << "\nresult.hpp "
				 "---------------------------------------------------------------"
				 "------\n\n"
			  << "Mathieu Stefani, 03 mai 2016\n"
			  << "Marie Eckert, 2024 (Modified for use in MFDASM)\n"
			  << "Licensed under the Apache License, Version 2.0.\n"
			  << "See <http://www.apache.org/licenses/>\n";
	std::exit(0);
}

int main(int argc, char **argv) {
	shared::cli::Argument<std::string> arg_verbosity("-v", "--verbosity");
	shared::cli::Argument<bool> arg_licenses("-l", "--licenses", true);
	shared::cli::Argument<std::string> arg_infile("-i");

	shared::cli::ArgumentParser parser;
	parser.addArgument(&arg_verbosity);
	parser.addArgument(&arg_licenses);
	parser.addArgument(&arg_infile);
	parser.parse(argc - 1, argv + 1);

	if(arg_licenses.get().value_or(false)) {
		licenses();
	}

	shared::Logger::stringSetLogLevel(arg_verbosity.get().value_or(""));

	/* start */

	std::cerr << "MFDEMU, emulator for the mfd0816 fantasy architecture\n"
			  << "Copyright (C) 2024  Marie Eckert\n\n";

	if(!arg_infile.get().has_value()) {
		logError() << "no input file specified! specify using \"-i <file>\"\n";
		return 1;
	}

	return 0;
}
