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

#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <Arg3P/Arg3P.hpp>

#include <shared/log.hpp>

#include <mfdasm/impl/assembler.hpp>
#include <mfdasm/impl/ast.hpp>
#include <mfdasm/impl/mri/mri.hpp>

namespace {

using namespace mfdasm;

constexpr const std::string_view VERSION = "v0.0 (develop)";

[[noreturn]] void licenses() {
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

int run(int argc, char **argv) {
	auto arg_help = Arg3P::Arg<bool>::make('h', "help", "display a help text");
	auto arg_verbosity =
		Arg3P::Arg<std::string>::make('v', "verbosity", "debug/info/warn/error/panic");
	auto arg_licenses = Arg3P::Arg<bool>::make('l', "licenses", "list licenses");
	auto arg_print_ast = Arg3P::Arg<bool>::make('a', "ast", "display the parsed AST");
	auto arg_outfile = Arg3P::Arg<std::string>::makeRequired('o');
	auto arg_infile = Arg3P::Arg<std::string>::makeRequired('i');
	auto arg_padded = Arg3P::Arg<bool>::make('p', "padded", "write a padded MRI");

	Arg3P::Parser<char *> parser{
		{arg_verbosity, arg_licenses, arg_print_ast, arg_outfile, arg_infile, arg_padded}};
	const std::optional<Arg3P::Error> error = parser(std::span<char *>(argv, argc).subspan(1));
	if(arg_help->get().value_or(false)) {
		std::cout << "SYNOPSIS: mfdasm " << parser.generateSynopsis() << "\n\n";
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

	/* start */

	std::cerr << "-- MFDASM, assembler for the mfd0816 fantasy architecture\n"
			  << "-- Version " << VERSION << "\n"
			  << "--\n"
			  << "-- Copyright (C) 2024  Marie Eckert\n\n";

	// NOLINTNEXTLINE(bugprone-unchecked-optional-access)
	const std::string infile = arg_infile->get().value();

	std::stringstream buffer;
	const std::ifstream instream(infile);

	buffer << instream.rdbuf();

	impl::Assembler asem;
	const Result<None, impl::AsmError> asm_res = asem.parseLines(buffer.str());

	if(asm_res.isErr()) {
		logError() << "Assembler: " << asm_res.unwrapErr().toString() << "\n";
		std::exit(1);
	}

	if(arg_print_ast->get().value_or(false)) {
		const std::optional<std::vector<impl::Statement>> maybe_ast = asem.ast();
		if(!maybe_ast.has_value()) {
			shared::panic("failed to retrieve asem's ast even though no error occured!");
		}
		const std::vector<impl::Statement> &ast = maybe_ast.value();
		std::cout << "[\n";
		for(const auto &statement: ast) {
			std::cout << statement.toString(1);
		}
		std::cout << "]\n";
	}

	const Result<impl::mri::SectionTable, impl::AsmError> bytes = asem.astToBytes();
	if(bytes.isErr()) {
		logError() << "Assembler (translation time): " << bytes.unwrapErr().toString() << "\n";
		std::exit(1);
	}

	const std::string outfile = arg_outfile->get().value_or(infile + ".mri");

	if(arg_padded->get().value_or(false)) {
		impl::mri::writePaddedMRI(outfile, bytes.unwrap(), false);
	} else {
		impl::mri::writeCompactMRI(outfile, bytes.unwrap(), false);
	}

	return 0;
}
}  // namespace

int main(int argc, char **argv) {
	shared::program_name = "mfdasm";

	try {
		return run(argc, argv);
	} catch(const std::exception &e) {
		shared::panic(std::string("uncaught exception escaped to main: ").append(e.what()));
	}
}
