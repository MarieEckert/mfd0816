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

#include <mfdasm/cli/args.hpp>
#include <mfdasm/impl/assembler.hpp>
#include <mfdasm/impl/ast.hpp>
#include <mfdasm/impl/mri/mri.hpp>
#include <mfdasm/log.hpp>

using namespace mfdasm;

[[noreturn]] static void licenses() {
	std::cerr
		<< "MFDASM -------------------------------------------------------------------------\n\n"
		<< "Copyright (C) 2024  Marie Eckert\n"
		<< "Licensed under the GPL v3 License.\n"
		<< "See <https://www.gnu.org/licenses/>.\n";
	std::cerr
		<< "\nresult.hpp ---------------------------------------------------------------------\n\n"
		<< "Mathieu Stefani, 03 mai 2016\n"
		<< "Marie Eckert, 2024 (Modified for use in MFDASM)\n"
		<< "Licensed under the Apache License, Version 2.0.\n"
		<< "See <http://www.apache.org/licenses/>\n";
	std::exit(0);
}

int main(int argc, char **argv) {
	cli::Argument<std::string> arg_verbosity("-v", "--verbosity");
	cli::Argument<bool> arg_licenses("-l", "--licenses", true);
	cli::Argument<bool> arg_print_ast("-a", "--ast", true);
	cli::Argument<std::string> arg_outfile("-o");
	cli::Argument<std::string> arg_infile("-i");
	cli::Argument<bool> arg_padded("-p", "--padded", true);

	cli::ArgumentParser parser;
	parser.addArgument(&arg_verbosity);
	parser.addArgument(&arg_licenses);
	parser.addArgument(&arg_print_ast);
	parser.addArgument(&arg_outfile);
	parser.addArgument(&arg_infile);
	parser.addArgument(&arg_padded);
	parser.parse(argc - 1, argv + 1);

	if(arg_licenses.get().value_or(false)) {
		licenses();
	}

	Logger::stringSetLogLevel(arg_verbosity.get().value_or(""));

	/* start */

	std::cerr << "MFDASM, assembler for the mfd0816 fantasy architecture\n"
			  << "Copyright (C) 2024  Marie Eckert\n\n";

	if(!arg_infile.get().has_value()) {
		logError() << "no input file specified! specify using \"-i <file>\"\n";
		return 1;
	}

	std::stringstream buffer;
	std::ifstream instream(arg_infile.get().value());

	buffer << instream.rdbuf();

	impl::Assembler asem;
	Result<None, impl::AsmError> asm_res = asem.parseLines(buffer.str());

	if(asm_res.isErr()) {
		logError() << "Assembler: " << asm_res.unwrapErr().toString() << "\n";
		std::exit(1);
	}

	if(arg_print_ast.get().value_or(false)) {
		std::vector<impl::Statement> ast = asem.ast().value();
		std::cout << "[\n";
		for(const auto &statement: ast) {
			std::cout << statement.toString(1);
		}
		std::cout << "]\n";
	}

	Result<impl::mri::SectionTable, impl::AsmError> bytes = asem.astToBytes();
	if(bytes.isErr()) {
		logError() << "Assembler (translation time): " << bytes.unwrapErr().toString() << "\n";
		std::exit(1);
	}

	const std::string outfile = arg_outfile.get().value_or(arg_infile.get().value() + ".mri");

	if(arg_padded.get().value_or(false)) {
		impl::mri::writePaddedMRI(outfile, bytes.unwrap(), false);
	} else {
		impl::mri::writeCompactMRI(outfile, bytes.unwrap(), false);
	}

	return 0;
}
