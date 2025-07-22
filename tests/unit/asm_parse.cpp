#include <algorithm>
#include <memory>
#include <optional>
#include <vector>

#include <shared/log.hpp>

#include <mfdasm/impl/assembler.hpp>
#include <mfdasm/impl/ast.hpp>
#include <mfdasm/impl/mri/section_table.hpp>

#include "mfdasm/impl/asmerror.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include "doctest/doctest.h"

using namespace mfdasm;

bool tryParseAsm(const std::string &text) {
	impl::Assembler asem;
	Result<None, impl::AsmError> asm_res = asem.parseLines(text);
	if(asm_res.isErr()) {
		FAIL("parsing failed: ", asm_res.unwrapErr().toString());
		return false;
	}

	return true;
}

TEST_CASE("instructions: no parameters") {
	CHECK(tryParseAsm(R"(section text at 0x1100
        ret
    )"));

	CHECK(tryParseAsm(R"(section text at 0x1100
        ret
        mov 100, acl
    )"));
}

TEST_CASE("instructions: multiple parameters") {
	CHECK(tryParseAsm(R"(section text at 0x1100
        mov acl, bcl
    )"));

	CHECK(tryParseAsm(R"(section text at 0x1100
        call test
    )"));

	CHECK(tryParseAsm(R"(section text at 0x1100
        call test
        mov acl, bcl
    )"));

	CHECK(tryParseAsm(R"(section text at 0x1100
        ld acl, 100
        call test
        ret
    )"));
}

TEST_CASE("generate and validate AST") {
	impl::Assembler asem;
	Result<None, impl::AsmError> parse_result = asem.parseLines(R"(section data at 0x0000
		times	16			dw	0xfeed
		define	terminator,	0
		ds		"Hellow, World!",	db	terminator
	section text at 0x1100
	_entry:	mov	0x1000,	sp			; initialise stack pointer
			ld		acl,	[sp]	; load the word from the top of the
									; stack into acl
			ld		bcl,	[[0x1330]]
			ld		ccl,	[[bcl]]

			st		0x1111,	[0x5000]	; first write
			neg		[0x5000]			; seconds write
			ld		ar,		[0x5000]
			not		[0x5000]			; third write
			or		[0x5000]
			st		ar,		[0x5000]	; fourth write
			and		0x1111
			st		ar,		[0x5000]	; fifth write
			rol		[0x5000], 5
			ror		[0x5000], 5

			call	test
			jmp		_entry
	test:	ret
	)");

	REQUIRE(parse_result.isOk());

	const std::optional<std::vector<impl::Statement>> maybe_ast = asem.ast();
	REQUIRE(maybe_ast.has_value());

	const std::vector<impl::Statement> ast = maybe_ast.value();
	REQUIRE(ast.size() > 0);

	std::vector<impl::Statement> expected_ast;
	expected_ast.push_back(
		impl::Statement(
			impl::Statement::Kind::SECTION,
			std::vector<std::shared_ptr<impl::ExpressionBase>>{
				std::make_shared<impl::Literal>(std::vector<u8>(8, 0), 1),
				std::make_shared<impl::Identifier>(impl::Identifier::Kind::SECTION, "data"),
			},
			1));
	expected_ast.push_back(
		impl::Statement(
			impl::Statement::Kind::DIRECTIVE, std::vector<std::shared_ptr<impl::ExpressionBase>>{},
			2,
			std::make_shared<impl::Directive>(
				impl::Directive::Kind::TIMES,
				std::vector<std::shared_ptr<impl::ExpressionBase>>{
					std::make_shared<impl::Literal>(
						std::vector<u8>{
							0x0,
							0x0,
							0x0,
							0x0,
							0x0,
							0x0,
							0x0,
							0x10,
						},
						2),
					std::make_shared<impl::StatementExpression>(impl::Statement(
						impl::Statement::Kind::DIRECTIVE,
						std::vector<std::shared_ptr<impl::ExpressionBase>>{}, 2,
						std::make_shared<impl::Directive>(
							impl::Directive::Kind::DW,
							std::vector<std::shared_ptr<impl::ExpressionBase>>{
								std::make_shared<impl::Literal>(
									std::vector<u8>{
										0x0,
										0x0,
										0x0,
										0x0,
										0x0,
										0x0,
										0xfe,
										0xed,
									},
									2)}))),
				})));

	CHECK_EQ(ast.size(), expected_ast.size());

	impl::ResolvalContext resolval_context;

	const u32 max = std::min(ast.size(), expected_ast.size());
	for(u32 ix = 0; ix < max; ix++) {
		const auto &statement = ast[ix];
		const auto &expected = expected_ast[ix];

		CHECK_EQ(statement.kind(), expected.kind());
		CHECK_EQ(statement.expressions().size(), expected.expressions().size());

		const u32 max_expression =
			std::min(statement.expressions().size(), expected.expressions().size());
		for(u32 ix = 0; ix < max_expression; ix++) {
			const auto &expression = statement.expressions()[ix];
			const auto &expected_expression = expected.expressions()[ix];

			CHECK_EQ(expression->kind(), expected_expression->kind());
			CHECK_EQ(expression->lineno(), expected_expression->lineno());
			CHECK_EQ(
				expression->resolveValue(resolval_context),
				expected_expression->resolveValue(resolval_context));
		}
	}

	std::cout << "[\n";
	for(const auto &statement: ast) {
		std::cout << statement.toString(1);
	}
	std::cout << "]\n";
}