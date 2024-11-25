#ifndef SASM_IMPL_SECTION_HPP
#define SASM_IMPL_SECTION_HPP

#include <vector>

#include <impl/asmerror.hpp>
#include <impl/token.hpp>
#include <result.hpp>
#include <typedefs.hpp>

namespace sasm::impl {

class Section {
   public:
	Section(const std::vector<Token> &tokens);

	Result<std::vector<u8>, AsmError> assemble();

   private:
	usize m_srcLine;

	u16 m_sectionLocation;

	std::vector<Token> m_tokens;
};

}  // namespace sasm::impl

#endif