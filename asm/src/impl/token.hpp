#ifndef SASM_IMPL_TOKEN_HPP
#define SASM_IMPL_TOKEN_HPP

namespace sasm::impl {
class Token {
	enum Type {
		END_OF_FILE,
		UNKNOWN,
		COMMA,
		DOUBLE_QUOTE,

		/* reserved keywords */
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