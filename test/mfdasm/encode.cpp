#include <memory>
#include <vector>

#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest/doctest.h>

#include <shared/log.hpp>

#include <mfdasm/impl/assembler.hpp>
#include <mfdasm/impl/ast.hpp>
#include <mfdasm/impl/mri/section_table.hpp>

#include "parse_helper.hpp"

using namespace mfdasm;

TEST_SUITE("Instruction Encoding") {
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
		std::shared_ptr<impl::mri::Section> section_ptr = test::mfdasm::tryParseAndTranslateAsm(R"(
			section a at 0
			mov 100, acl
		)");
		REQUIRE(section_ptr != nullptr);
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
		section_ptr = test::mfdasm::tryParseAndTranslateAsm(R"(
			section a at 0
			mov bcl, acl
		)");
		REQUIRE(section_ptr != nullptr);
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
		std::shared_ptr<impl::mri::Section> section_ptr = test::mfdasm::tryParseAndTranslateAsm(R"(
			section a at 0
			st 100, [acl]
		)");
		REQUIRE(section_ptr != nullptr);
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
		std::shared_ptr<impl::mri::Section> section_ptr = test::mfdasm::tryParseAndTranslateAsm(R"(
			section a at 0
			st 100, [[acl]]
		)");
		REQUIRE(section_ptr != nullptr);
		CHECK(section_ptr->offset == 0);
		CHECK(
			section_ptr->data ==
			std::vector<u8>{impl::Instruction::ST, 0b00000000 | 0b1010, 0x00, 0x64, 0x02});
	}
}