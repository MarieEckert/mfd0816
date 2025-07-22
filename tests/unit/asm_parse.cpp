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

bool tryParseAsm(const std::string &text) {
	impl::Assembler asem;
	Result<None, impl::AsmError> asm_res = asem.parseLines(text);
	if(asm_res.isErr()) {
		FAIL("parsing failed: ", asm_res.unwrapErr().toString());
		return false;
	}

	return true;
}

TEST_CASE("instructions: no parameters") {
	CHECK(tryParseAsm(R"(section text at 0x1100
        ret
    )"));

	CHECK(tryParseAsm(R"(section text at 0x1100
        ret
        mov 100, acl
    )"));
}

TEST_CASE("instructions: multiple parameters") {
	CHECK(tryParseAsm(R"(section text at 0x1100
        mov acl, bcl
    )"));

	CHECK(tryParseAsm(R"(section text at 0x1100
        call test
    )"));

	CHECK(tryParseAsm(R"(section text at 0x1100
        call test
        mov acl, bcl
    )"));

	CHECK(tryParseAsm(R"(section text at 0x1100
        ld acl, 100
        call test
        ret
    )"));
}