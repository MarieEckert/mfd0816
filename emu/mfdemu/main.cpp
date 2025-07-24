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

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

#include <shared/cli/args.hpp>
#include <shared/log.hpp>
#include <shared/panic.hpp>

#include <mfdemu/impl/system.hpp>
#include <mfdemu/mri.hpp>

#define VERSION "0.0 (develop)"

using namespace mfdemu;

[[noreturn]] static void licenses() {
	std::cerr << "MFDEMU "
				 "---------------------------------------------------------------"
				 "----------\n\n"
			  << "Copyright (C) 2024  Marie Eckert\n"
			  << "Licensed under the GPL v3 License.\n"
			  << "See <https://www.gnu.org/licenses/>.\n";
	std::exit(0);
}

int main(int argc, char **argv) {
	shared::program_name = "mfdemu";

	shared::cli::Argument<std::string> arg_verbosity("-v", "--verbosity");
	shared::cli::Argument<bool> arg_licenses("-l", "--licenses", true);
	shared::cli::Argument<std::string> arg_infile("-i");
	shared::cli::Argument<u64> arg_cycle_span("-c", "--cycle-span");

	shared::cli::ArgumentParser parser;
	parser.addArgument(&arg_verbosity);
	parser.addArgument(&arg_licenses);
	parser.addArgument(&arg_infile);
	parser.addArgument(&arg_cycle_span);
	parser.parse(argc - 1, argv + 1);

	if(arg_licenses.get().value_or(false)) {
		licenses();
	}

	shared::Logger::stringSetLogLevel(arg_verbosity.get().value_or(""));

	constexpr u64 default_cycle_span = 1000; /* ~10MHz */
	const u64 cycle_span = arg_cycle_span.get().value_or(default_cycle_span);

	std::cerr << "MFDEMU, emulator for the mfd0816 fantasy architecture\n"
			  << "Copyright (C) 2024  Marie Eckert\n\n";

	if(!arg_infile.get().has_value()) {
		logError() << "no input file specified! specify using \"-i <file>\"\n";
		return 1;
	}

	std::ifstream stream(arg_infile.get().value(), std::ios::in | std::ios::binary);
	std::vector<u8> contents(
		(std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

	impl::System the_system(cycle_span, UINT16_MAX);
	the_system.setMainMemoryData(parseMRIFromBytes(contents));
	the_system.run();

	return 0;
}
