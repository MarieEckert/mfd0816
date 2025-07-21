#include <cstdint>
#include <filesystem>
#include <fstream>
#include <vector>

#include <unistd.h>

#include <shared/log.hpp>

#include <mfdasm/impl/assembler.hpp>
#include <mfdasm/impl/ast.hpp>
#include <mfdasm/impl/mri/mri.hpp>
#include <mfdasm/impl/mri/section_table.hpp>

#include <mfdemu/mri.hpp>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include "doctest/doctest.h"

using namespace mfdasm;

TEST_CASE("generate & parse mri") {
	impl::Assembler asem;
	Result<None, impl::AsmError> asm_res = asem.parseLines(R"(
	section garbage at 0x0000
		times 512 db 0xff
	section text at 0x1100
		mov 100, acl
		st acl, [0x1234]
	section reset_vector at 0xffe4
		ds "RESET VECTOR; NO TOUCHING!"
		reset_vector:	dw text
	)");
	if(asm_res.isErr()) {
		FAIL("parsing failed: ", asm_res.unwrapErr().toString());
	}

	Result<impl::mri::SectionTable, impl::AsmError> maybe_bytes = asem.astToBytes();
	if(maybe_bytes.isErr()) {
		FAIL("assembling failed: ", maybe_bytes.unwrapErr().toString());
	}

	impl::mri::SectionTable sections = maybe_bytes.unwrap();
	CHECK(sections.sectionMap().size() == 3);
	CHECK(sections.findSectionByAddress(0x0000) != nullptr);
	CHECK(sections.findSectionByAddress(0x1100) != nullptr);
	CHECK(sections.findSectionByAddress(0xffe4) != nullptr);

	const int pid = getpid();
	const std::string tmp_file = "/tmp/asm_generate_mri_test." + std::to_string(pid);
	impl::mri::writeCompactMRI(tmp_file, sections, false);

	REQUIRE(std::filesystem::exists(tmp_file));

	std::ifstream stream(tmp_file);
	REQUIRE(stream.is_open());
	std::vector<u8> mri_bytes(
		(std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
	std::vector<u8> parsed_mri = mfdemu::parseMRIFromBytes(mri_bytes);
	REQUIRE(parsed_mri.size() == UINT16_MAX);
}
