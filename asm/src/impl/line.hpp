#ifndef SASM_IMPL_LINE_HPP
#define SASM_IMPL_LINE_HPP

#include <impl/asmerror.hpp>
#include <result.hpp>
#include <typedefs.hpp>

namespace sasm::impl {
class Line {
   public:
	Result<None, AsmError> parse(const std::string &value);
	Result<std::vector<u8>, AsmError> assemble();
};
}  // namespace sasm::impl

#endif