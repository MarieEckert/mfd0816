#include <memory>
#include <vector>

#include <shared/log.hpp>

#include <mfdasm/impl/assembler.hpp>
#include <mfdasm/impl/ast.hpp>
#include <mfdasm/impl/mri/section_table.hpp>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include "doctest/doctest.h"

using namespace mfdasm;

std::shared_ptr<impl::mri::Section> tryParseAsm(const std::string &text) {
	impl::Assembler asem;
	Result<None, impl::AsmError> asm_res = asem.parseLines(text);
	if(asm_res.isErr()) {
		FAIL("parsing failed: ", asm_res.unwrapErr().toString());
	}

	Result<impl::mri::SectionTable, impl::AsmError> maybe_bytes = asem.astToBytes();
	if(maybe_bytes.isErr()) {
		FAIL("assembling failed: ", maybe_bytes.unwrapErr().toString());
	}

	impl::mri::SectionTable bytes = maybe_bytes.unwrap();
	const std::shared_ptr<impl::mri::Section> section_ptr = bytes.findSectionByAddress(0);
	REQUIRE(section_ptr != nullptr);

	return section_ptr;
}

TEST_CASE("encode immediates") {
	/* expected:
	 * > Section
	 * Offset 0, Size 5
	 * > Bytes
	 * 0: 0x1c (MOV)
	 * 1: 0b00001000 (Op.1 IMM, Op.2 REG)
	 * 2: 0x00 (Op.1 HI)
	 * 3: 0x64 (Op.2 LO)
	 * 4: 0x02 (Op.2 REG, ACL)
	 */
	std::shared_ptr<impl::mri::Section> section_ptr = tryParseAsm(R"(
		section a at 0
		mov 100, acl
	)");
	CHECK(section_ptr->offset == 0);
	CHECK(
		section_ptr->data ==
		std::vector<u8>{impl::Instruction::MOV, 0b00000000 | 0b1000, 0x00, 0x64, 0x02});

	/* expected:
	 * > Section
	 * Offset 0, Size 4
	 * > Bytes
	 * 0: 0x1c (MOV)
	 * 1: 0b10001000 (Op.1 REG, Op.2 REG)
	 * 2: 0x05 (Op.1 REG, BCL)
	 * 3: 0x02 (Op.2 REG, ACL)
	 */
	section_ptr = tryParseAsm(R"(
		section a at 0
		mov bcl, acl
	)");
	CHECK(section_ptr->offset == 0);
	CHECK(section_ptr->data == std::vector<u8>{impl::Instruction::MOV, 0b10001000, 0x05, 0x02});
}

TEST_CASE("encode directs") {
	/* expected:
	 * > Section
	 * Offset 0, Size 5
	 * > Bytes
	 * 0: 0x2a (MOV)
	 * 1: 0b00001001 (Op.1 IMM, Op.2 DIR REG)
	 * 2: 0x00 (Op.1 HI)
	 * 3: 0x64 (Op.2 LO)
	 * 4: 0x02 (Op.2 REG, ACL)
	 */
	std::shared_ptr<impl::mri::Section> section_ptr = tryParseAsm(R"(
		section a at 0
		st 100, [acl]
	)");
	CHECK(section_ptr->offset == 0);
	CHECK(
		section_ptr->data ==
		std::vector<u8>{impl::Instruction::ST, 0b00000000 | 0b1001, 0x00, 0x64, 0x02});
}

TEST_CASE("encode indirects") {
	/* expected:
	 * > Section
	 * Offset 0, Size 5
	 * > Bytes
	 * 0: 0x2a (MOV)
	 * 1: 0b00001010 (Op.1 IMM, Op.2 IND REG)
	 * 2: 0x00 (Op.1 HI)
	 * 3: 0x64 (Op.2 LO)
	 * 4: 0x02 (Op.2 REG, ACL)
	 */
	std::shared_ptr<impl::mri::Section> section_ptr = tryParseAsm(R"(
		section a at 0
		st 100, [[acl]]
	)");
	CHECK(section_ptr->offset == 0);
	CHECK(
		section_ptr->data ==
		std::vector<u8>{impl::Instruction::ST, 0b00000000 | 0b1010, 0x00, 0x64, 0x02});
}