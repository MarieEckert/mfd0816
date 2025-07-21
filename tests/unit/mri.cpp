#include <algorithm>
#include <filesystem>
#include <fstream>
#include <ranges>
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

TEST_CASE("generate & validate mri") {
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
	const std::vector<u8> mri_bytes(
		(std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
	stream.close();

	/* ensure that the header and section table are correct */
	CHECK(/* MAGIC */
		  std::ranges::equal(
			  mri_bytes | std::views::take(4), std::vector<u8>{'M', 'R', 'I', 0x00}));
	CHECK(/* VERSION */
		  std::ranges::equal(
			  mri_bytes | std::views::drop(4) | std::views::take(2), std::vector<u8>{0x01, 0x00}));
	CHECK(/* FLAGS (compact)*/
		  std::ranges::equal(
			  mri_bytes | std::views::drop(6) | std::views::take(2), std::vector<u8>{0x01, 0x00}));
	CHECK(/* FILESIZE */
		  std::ranges::equal(
			  mri_bytes | std::views::drop(8) | std::views::take(4),
			  shared::intops::u64ToBytes(mri_bytes.size()) | std::views::drop(4)));
	CHECK(/* DATA OFFSET */
		  std::ranges::equal(
			  mri_bytes | std::views::drop(12) | std::views::take(4),
			  shared::intops::u64ToBytes(sizeof(shared::mri_types::Header)) | std::views::drop(4)));
	CHECK(/* SECTION TABLE: ENTRY COUNT*/
		  std::ranges::equal(
			  mri_bytes | std::views::drop(16) | std::views::take(4),
			  std::vector<u8>{0x00, 0x00, 0x00, 0x03}));
	/* SECTION TABLE ENTRIES*/
	CHECK(/* 01 */
		  std::ranges::equal(
			  mri_bytes | std::views::drop(20) | std::views::take(8),
			  std::vector<u8>{
				  0x00, 0x00, 0x00, 0x2c, /* FILE OFFSET */
				  0x00, 0x00,			  /* LOAD ADDRESS */
				  0x02, 0x00,			  /* LENGTH */
			  }));
	CHECK(/* 02 */
		  std::ranges::equal(
			  mri_bytes | std::views::drop(28) | std::views::take(8),
			  std::vector<u8>{
				  0x00, 0x00, 0x02, 0x2c, /* FILE OFFSET */
				  0x11, 0x00,			  /* LOAD ADDRESS */
				  0x00, 0x0a,			  /* LENGTH */
			  }));
	CHECK(/* 03 */
		  std::ranges::equal(
			  mri_bytes | std::views::drop(36) | std::views::take(8),
			  std::vector<u8>{
				  0x00, 0x00, 0x02, 0x36, /* FILE OFFSET */
				  0xff, 0xe4,			  /* LOAD ADDRESS */
				  0x00, 0x1c,			  /* LENGTH */
			  }));
	/* END SECTION TABLE ENTRIES */

	/* ensure that some of the expected data is there */
	CHECK(/* BULLSHIT DATA */
		  std::ranges::equal(
			  mri_bytes | std::views::drop(0x2c) | std::views::take(512),
			  std::vector<u8>(512, 0xff)));

	std::remove(tmp_file.c_str());
}
