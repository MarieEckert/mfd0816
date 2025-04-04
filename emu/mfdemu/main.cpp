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

#include <cassert>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <thread>

#include <shared/cli/args.hpp>
#include <shared/log.hpp>

#include <mfdemu/impl/aio_device.hpp>
#include <mfdemu/impl/cpu.hpp>
#include <mfdemu/mri.hpp>

#include "mfdemu/impl/bus_device.hpp"

#define VERSION "0.0 (develop)"

using namespace mfdemu;

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

	/* start */

	std::cerr << "MFDEMU, emulator for the mfd0816 fantasy architecture\n"
			  << "Copyright (C) 2024  Marie Eckert\n\n";

	if(!arg_infile.get().has_value()) {
		logError() << "no input file specified! specify using \"-i <file>\"\n";
		return 1;
	}

	std::ifstream stream(arg_infile.get().value(), std::ios::in | std::ios::binary);
	std::vector<u8> contents(
		(std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

	impl::Cpu cpu;
	impl::AioDevice test_dev(false, UINT16_MAX);

	test_dev.setData(parseMRIFromBytes(contents));

	cpu.connectAddressDevice(
		std::static_pointer_cast<impl::BaseBusDevice>(std::make_shared<impl::AioDevice>(test_dev)));

	cpu.reset = true;

	struct timespec ts;
	u64 last_time = 0;
	u64 cycle_count = 0;

	/* used for speed display */
	u64 last_clock_time = 0;
	u64 clock_cycles = 0;

	while(true) {
		if(cycle_count == 1) {
			cpu.reset = false;
		}

		assert(clock_gettime(CLOCK_MONOTONIC, &ts) == 0);
		const u64 current_time = (ts.tv_sec * (1000 * 1000 * 1000)) + ts.tv_nsec;

		if(current_time - last_clock_time >= 1000 * 1000 * 1000) {
			std::cout << "\rSpeed: " << clock_cycles << " cps" << std::flush;
			clock_cycles = 0;
			last_clock_time = current_time;
		}

		if(current_time - last_time < cycle_span) {
			continue;
		}

		last_time = current_time;

		cycle_count++;
		clock_cycles++;

		cpu.iclck();
		// std::this_thread::sleep_for(std::chrono::milliseconds(50));
		// logDebug() << "cycle " << cycle_count << "\r";
	}

	return 0;
}
