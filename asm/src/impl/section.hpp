#ifndef SASM_IMPL_SECTION_HPP
#define SASM_IMPL_SECTION_HPP

#include <optional>
#include <string>
#include <vector>

#include <impl/asmerror.hpp>
#include <impl/line.hpp>
#include <result.hpp>
#include <typedefs.hpp>

namespace sasm::impl {

class Section {
   public:
	Result<std::nullopt_t, AsmError> parse(const std::vector<std::string> &line);

	Result<std::vector<u8>, AsmError> assemble();

   private:
	usize m_srcLine;

	u16 m_sectionLocation;

	std::vector<Line> m_lines;
};

}  // namespace sasm::impl

#endif