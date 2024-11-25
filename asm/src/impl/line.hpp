#ifndef SASM_IMPL_LINE_HPP
#define SASM_IMPL_LINE_HPP

#include <impl/asmerror.hpp>
#include <impl/token.hpp>
#include <result.hpp>
#include <typedefs.hpp>

namespace sasm::impl {
class Line {
   public:
	Line(const std::vector<Token> &tokens);
	Result<std::vector<u8>, AsmError> assemble();
};
}  // namespace sasm::impl

#endif