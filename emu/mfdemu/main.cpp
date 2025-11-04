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

#include <Arg3P/Arg3P.hpp>

#include <shared/log.hpp>
#include <shared/panic.hpp>

#include <mfdemu/impl/system.hpp>
#include <mfdemu/mri.hpp>

namespace {

using namespace mfdemu;

constexpr const std::string_view VERSION = "v0.0 (develop)";

[[noreturn]] void licenses() {
	std::cerr << "MFDEMU "
				 "---------------------------------------------------------------"
				 "----------\n\n"
			  << "Copyright (C) 2024  Marie Eckert\n"
			  << "Licensed under the GPL v3 License.\n"
			  << "See <https://www.gnu.org/licenses/>.\n";
	std::exit(0);
}

int run(int argc, char **argv) {
	auto arg_help = Arg3P::Arg<bool>::make('h', "help", "show a help text");
	auto arg_verbosity =
		Arg3P::Arg<std::string>::make('v', "verbosity", "debug/info/warn/error/panic");
	;
	auto arg_licenses = Arg3P::Arg<bool>::make('l', "licenses", "list licenses");
	auto arg_infile = Arg3P::Arg<std::string>::makeRequired('i');
	auto arg_cycle_span =
		Arg3P::Arg<u64>::make('c', "cycle-span", "specify the span of each cycle in nanoseconds");

	Arg3P::Parser<char *> parser{
		{arg_help, arg_verbosity, arg_licenses, arg_infile, arg_cycle_span}};
	const std::optional<Arg3P::Error> error = parser(std::span<char *>(argv, argc).subspan(1));
	if(arg_help->get().value_or(false)) {
		std::cout << "SYNOPSIS: mfdemu " << parser.generateSynopsis() << "\n\n";
		std::cout << parser.generateHelp() << "\n";
		return 0;
	}

	if(error.has_value()) {
		std::cerr << "Error parsing arguments: " << errorName(error.value().error) << ": "
				  << error.value().message << "\n";
		return 1;
	}

	if(arg_licenses->get().value_or(false)) {
		licenses();
	}

	shared::Logger::stringSetLogLevel(arg_verbosity->get().value_or(""));

	constexpr u64 DEFAULT_CYCLE_SPAN = 1000; /* ~10MHz */
	const u64 cycle_span = arg_cycle_span->get().value_or(DEFAULT_CYCLE_SPAN);

	std::cerr << "-- MFDEMU, emulator for the mfd0816 fantasy architecture\n"
			  << "-- Version " << VERSION << "\n"
			  << "--\n"
			  << "Copyright (C) 2024  Marie Eckert\n\n";

	// NOLINTNEXTLINE(bugprone-unchecked-optional-access)
	const std::string infile = arg_infile->get().value();

	std::ifstream stream(infile, std::ios::in | std::ios::binary);
	const std::vector<u8> contents(
		(std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

	impl::System the_system(cycle_span, UINT16_MAX);
	the_system.setMainMemoryData(parseMRIFromBytes(contents));
	the_system.run();

	return 0;
}

}  // namespace

int main(int argc, char **argv) {
	shared::program_name = "mfdemu";

	try {
		return run(argc, argv);
	} catch(const std::exception &e) {
		shared::panic(std::string("uncaught exception escaped to main: ").append(e.what()));
	}
}
