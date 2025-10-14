#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest/doctest.h>

#include "parse_helper.hpp"
#include <mfdasm/impl/assembler.hpp>

namespace test::mfdasm {
bool tryParseAsm(const std::string &text, ::mfdasm::impl::Assembler &asem) {
	::mfdasm::Result<::mfdasm::None, ::mfdasm::impl::AsmError> asm_res = asem.parseLines(text);
	if(asm_res.isErr()) {
		FAIL_CHECK("parsing failed: ", asm_res.unwrapErr().toString());
		return false;
	}

	return true;
}

std::shared_ptr<::mfdasm::impl::mri::Section> tryParseAndTranslateAsm(const std::string &text) {
	::mfdasm::impl::Assembler asem;
	const bool parse_result = tryParseAsm(text, asem);
	if(!parse_result) {
		return nullptr;
	}

	::mfdasm::Result<::mfdasm::impl::mri::SectionTable, ::mfdasm::impl::AsmError> maybe_bytes =
		asem.astToBytes();
	if(maybe_bytes.isErr()) {
		FAIL_CHECK("assembling failed: ", maybe_bytes.unwrapErr().toString());
		return nullptr;
	}

	::mfdasm::impl::mri::SectionTable bytes = maybe_bytes.unwrap();
	const std::shared_ptr<::mfdasm::impl::mri::Section> section_ptr = bytes.findSectionByAddress(0);
	REQUIRE(section_ptr != nullptr);

	return section_ptr;
}
}  // namespace test::mfdasm