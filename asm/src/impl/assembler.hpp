#ifndef SASM_IMPL_ASSEMBLER_HPP
#define SASM_IMPL_ASSEMBLER_HPP

#include <impl/asmerror.hpp>
#include <impl/section.hpp>
#include <result.hpp>

namespace sasm::impl {

class Assembler {
   public:
	Result<None, AsmError> parseLines(const std::vector<std::string> &lines);

   private:
	std::vector<Section> m_sections;
};

}  // namespace sasm::impl

#endif