#ifndef SASM_IMPL_TOKEN_HPP
#define SASM_IMPL_TOKEN_HPP

namespace sasm::impl {
class Token {
	enum Type {
		UNKNOWN,
		COMMA,
		DOUBLE_QUOTE,

		/* reserved keyword s*/
		DB,
		DW,
		DD,
		TIMES,
		_HERE,
		SECTION,
		SP,
		IP,
		FL,
		AL,
		AH,
		ACL,
		BL,
		BH,
		BCL,
		CL,
		CH,
		CCL,
		DL,
		DH,
		DCL,
		ADDRESSING,
		RELATIVE,
		ABSOLUTE,
		AT
	};
};
}  // namespace sasm::impl

#endif