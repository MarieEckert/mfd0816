#ifndef TEST_ASM_PARSE_HELPER_HPP
#define TEST_ASM_PARSE_HELPER_HPP

#include <mfdasm/impl/assembler.hpp>

namespace test::mfdasm {
bool tryParseAsm(const std::string &text, ::mfdasm::impl::Assembler &asem);
std::shared_ptr<::mfdasm::impl::mri::Section> tryParseAndTranslateAsm(const std::string &text);
}  // namespace test::mfdasm

#endif