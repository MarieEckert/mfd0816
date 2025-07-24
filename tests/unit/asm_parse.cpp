#include <algorithm>
#include <memory>
#include <optional>
#include <vector>

#include <shared/log.hpp>

#include <mfdasm/impl/assembler.hpp>
#include <mfdasm/impl/ast.hpp>
#include <mfdasm/impl/mri/section_table.hpp>

#include "mfdasm/impl/asmerror.hpp"
#include "mfdasm/impl/token.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include "doctest/doctest.h"

using namespace mfdasm;
using namespace mfdasm::impl;

bool tryParseAsm(const std::string &text) {
	Assembler asem;
	Result<None, AsmError> asm_res = asem.parseLines(text);
	if(asm_res.isErr()) {
		FAIL("parsing failed: ", asm_res.unwrapErr().toString());
		return false;
	}

	return true;
}

TEST_CASE("comments") {
	CHECK(tryParseAsm(R"(section text at 0x1100
; Extemplo libyae magnas it fama per urbes,
; Fama, malum qua non aliud velocius ullum.
; Mobilitate viget virisque adquirit eundo;
; Parva metu primo, mox sese attollit in auras.
; Ingrediturque solo et caput inter nubila condit.
section data at 0xd000
	)"));
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
	Assembler asem;
	Result<None, AsmError> parse_result = asem.parseLines(R"(section data at 0x0000
		times	16			dw	0xfeed
		define	terminator,	0
		ds		"Hello, World!"	db	terminator
	section text at 0x1100
	_entry:	mov		0x1000,		sp
			ld		acl,		[sp]
			ld		bcl,		[[0x1330]]
			ld		ccl,		[[bcl]]
			st		0x1111,		[0x5000]
			neg		[0x5000]
			rol		[0x5000],	5
			call	test
	test:	ret
	)");

	REQUIRE(parse_result.isOk());

	const std::optional<std::vector<Statement>> maybe_ast = asem.ast();
	REQUIRE(maybe_ast.has_value());

	const std::vector<Statement> ast = maybe_ast.value();
	REQUIRE(ast.size() > 0);

	std::vector<Statement> expected_ast;
	expected_ast.push_back(Statement(
		Statement::SECTION,
		Expressions{
			std::make_shared<Identifier>(Identifier::SECTION, "data", 1),
			std::make_shared<Literal>(std::vector<u8>(8, 0), 1),
		},
		1));
	expected_ast.push_back(Statement(
		Statement::DIRECTIVE, Expressions{}, 2,
		std::make_shared<Directive>(
			Directive::TIMES,
			Expressions{
				std::make_shared<Literal>(
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
				std::make_shared<StatementExpression>(
					Statement(
						Statement::DIRECTIVE, Expressions{}, 2,
						std::make_shared<Directive>(
							Directive::DW,
							Expressions{
								std::make_shared<Literal>(
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
									2),
							},
							2)),
					2),
			},
			2)));
	expected_ast.push_back(Statement(
		Statement::DIRECTIVE, Expressions{}, 3,
		std::make_shared<Directive>(
			Directive::DEFINE,
			Expressions{
				std::make_shared<Identifier>(Identifier::LABEL, "terminator", 3),
				std::make_shared<Literal>(std::vector<u8>(8, 0), 3),
			},
			3)));
	expected_ast.push_back(Statement(
		Statement::DIRECTIVE, Expressions{}, 4,
		std::make_shared<Directive>(
			Directive::DS,
			Expressions{
				std::make_shared<Literal>(
					Token(Token::Type::STRING, 4, "Hello, World!").toBytes(), 4),
			},
			4)));
	expected_ast.push_back(Statement(
		Statement::DIRECTIVE, Expressions{}, 4,
		std::make_shared<Directive>(
			Directive::DB,
			Expressions{
				std::make_shared<Identifier>(Identifier::LABEL, "terminator", 4),
			},
			4)));
	expected_ast.push_back(Statement(
		Statement::SECTION,
		Expressions{
			std::make_shared<Identifier>(Identifier::SECTION, "text", 5),
			std::make_shared<Literal>(
				std::vector<u8>{
					0x0,
					0x0,
					0x0,
					0x0,
					0x0,
					0x0,
					0x11,
					0x0,
				},
				5),
		},
		5));
	expected_ast.push_back(Statement(
		Statement::LABEL,
		Expressions{
			std::make_shared<Identifier>(Identifier::LABEL, "_entry", 6),
		},
		6));
	expected_ast.push_back(Statement(
		Statement::INSTRUCTION, Expressions{}, 6,
		std::make_shared<Instruction>(
			Instruction::MOV,
			Expressions{
				std::make_shared<Literal>(
					std::vector<u8>{
						0x0,
						0x0,
						0x0,
						0x0,
						0x0,
						0x0,
						0x10,
						0x0,
					},
					6),
				std::make_shared<Register>(Register::SP, 6),
			},
			6)));
	expected_ast.push_back(Statement(
		Statement::INSTRUCTION, Expressions{}, 7,
		std::make_shared<Instruction>(
			Instruction::LD,
			Expressions{
				std::make_shared<Register>(Register::ACL, 7),
				std::make_shared<DirectAddress>(
					DirectAddress::REGISTER, std::make_shared<Register>(Register::SP, 7), 7),
			},
			7)));
	expected_ast.push_back(Statement(
		Statement::INSTRUCTION, Expressions{}, 8,
		std::make_shared<Instruction>(
			Instruction::LD,
			Expressions{
				std::make_shared<Register>(Register::BCL, 8),
				std::make_shared<IndirectAddress>(
					IndirectAddress::MEMORY,
					std::make_shared<Literal>(
						std::vector<u8>{
							0x0,
							0x0,
							0x0,
							0x0,
							0x0,
							0x0,
							0x13,
							0x30,
						},
						8),
					8),
			},
			8)));
	expected_ast.push_back(Statement(
		Statement::INSTRUCTION, Expressions{}, 9,
		std::make_shared<Instruction>(
			Instruction::LD,
			Expressions{
				std::make_shared<Register>(Register::CCL, 9),
				std::make_shared<IndirectAddress>(
					IndirectAddress::REGISTER, std::make_shared<Register>(Register::BCL, 9), 9),
			},
			9)));
	expected_ast.push_back(Statement(
		Statement::INSTRUCTION, Expressions{}, 10,
		std::make_shared<Instruction>(
			Instruction::ST,
			Expressions{
				std::make_shared<Literal>(
					std::vector<u8>{
						0x0,
						0x0,
						0x0,
						0x0,
						0x0,
						0x0,
						0x11,
						0x11,
					},
					10),
				std::make_shared<DirectAddress>(
					DirectAddress::MEMORY,
					std::make_shared<Literal>(
						std::vector<u8>{
							0x0,
							0x0,
							0x0,
							0x0,
							0x0,
							0x0,
							0x50,
							0x0,
						},
						10),
					10),
			},
			10)));
	expected_ast.push_back(Statement(
		Statement::INSTRUCTION, Expressions{}, 11,
		std::make_shared<Instruction>(
			Instruction::NEG,
			Expressions{
				std::make_shared<DirectAddress>(
					DirectAddress::MEMORY,
					std::make_shared<Literal>(
						std::vector<u8>{
							0x0,
							0x0,
							0x0,
							0x0,
							0x0,
							0x0,
							0x50,
							0x0,
						},
						11),
					11),
			},
			11)));
	expected_ast.push_back(Statement(
		Statement::INSTRUCTION, Expressions{}, 12,
		std::make_shared<Instruction>(
			Instruction::ROL,
			Expressions{
				std::make_shared<DirectAddress>(
					DirectAddress::MEMORY,
					std::make_shared<Literal>(
						std::vector<u8>{
							0x0,
							0x0,
							0x0,
							0x0,
							0x0,
							0x0,
							0x50,
							0x0,
						},
						12),
					12),
				std::make_shared<Literal>(
					std::vector<u8>{
						0x0,
						0x0,
						0x0,
						0x0,
						0x0,
						0x0,
						0x0,
						0x5,
					},
					12)},
			12)));
	expected_ast.push_back(Statement(
		Statement::INSTRUCTION, Expressions{}, 13,
		std::make_shared<Instruction>(
			Instruction::CALL,
			Expressions{
				std::make_shared<Identifier>(Identifier::LABEL, "test", 13),
			},
			13)));
	expected_ast.push_back(Statement(
		Statement::LABEL,
		Expressions{
			std::make_shared<Identifier>(Identifier::LABEL, "test", 14),
		},
		14));
	expected_ast.push_back(Statement(
		Statement::INSTRUCTION, Expressions{}, 15,
		std::make_shared<Instruction>(Instruction::RET, Expressions{}, 15)));

	CHECK_EQ(ast.size(), expected_ast.size());

	ResolvalContext resolval_context;

	const u32 max = std::min(ast.size(), expected_ast.size());
	for(u32 ix = 0; ix < max; ix++) {
		const auto &statement = ast[ix];
		const auto &expected = expected_ast[ix];

		CHECK_EQ(statement.toString(1), expected.toString(1));
	}
}