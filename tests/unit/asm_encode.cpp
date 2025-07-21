#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include <mfdasm/impl/assembler.hpp>

TEST_CASE("encode immediates") {
  using namespace mfdasm;
  impl::Assembler asem;
  Result<None, impl::AsmError> asm_res = asem.parseLines("mov acl, 100");
}