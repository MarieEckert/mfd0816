/*
 * Copyright (C) 2024  Marie Eckert
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * @file ast.hpp
 * @brief Type definitions for the Abstract Syntax Tree used by mfdasm.
 *
 * # AST definition
 * @note Any part of a statement or expression which is surrounded by parenthesis, e.g.
 * ```
 *      (|> token(at))
 * ```
 * is not represented in the final AST but required within the input for that statement
 * or expression.
 *
 * ```
 * statement (section, Statement::SECTION)
 *     |> expression <identifier>
 *    (|> token (at))
 *     |> expression <number literal>
 *
 * statement (ld, Statement::INSTRUCTION{Instruction::LD})
 *     |> Expression::REGISTER
 *    (|> token (,))
 *     |> expression
 *
 * expression (literal, Expression::LITERAL)
 *     |> <literal>
 *
 * expression (identifier, Expression::IDENTIFIER)
 *     |> <identifier name>
 *
 * expression (direct addressing, Expression::DIRECT_ADDRESS)
 *    (|> token ([))
 *     |> expression (literal OR identifer)
 *    (|> token (]))
 *
 * expression (indirect addressing, Expression::INDIRECT_ADDRESS)
 *    (|> token ([))
 *    (|> token ([))
 *     |> expression (literal OR identifer)
 *    (|> token (]))
 *    (|> token (]))
 * ```
 *
 * # Parsed example
 * @note This part is really just a note to myself.
 *
 * Input:
 * ```asm
 * section code at 0x1000
 * _entry: jmp _entry
 * ```
 *
 * Output:
 * ```
 * [
 *    Statement {
 *        kind: section
 *        expressions: [
 *            Expression {
 *                kind: identifier
 *                Identifier {
 *                    kind: section
 *                    name: "code"
 *                }
 *            }
 *            Expression {
 *                kind: literal
 *                Literal {
 *                    value: bytes [0x10, 0x00]
 *                }
 *            }
 *        ]
 *    }
 *    Statement {
 *        kind: label
 *        expressions: [
 *            Expression {
 *                kind: identifier
 *                Identifier {
 *                    kind: label
 *                    name: "_entry"
 *                }
 *            }
 *        ]
 *    },
 *    Statement {
 *        kind: instruction
 *        Instruction {
 *            kind: JMP
 *            expressions: [
 *                Expression {
 *                    kind: identifier
 *                    Identifier {
 *                        kind: label
 *                        name: "_entry"
 *                    }
 *                }
 *            ]
 *        }
 *    },
 *    Statement {
 *        kind: directive
 *        Directive {
 *            kind: TIMES
 *            expressions: [
 *                Expression {
 *                    kind: literal
 *                    value: 10
 *                },
 *                Expression {
 *                    kind: statement
 *                    value: Statement {
 *                        kind: directive
 *                        Directive {
 *                            kind: DW
 *                            expressions: [
 *                                Expression {
 *                                    kind: literal
 *                                    value: 0
 *                                }
 *                            ]
 *                        }
 *                    }
 *                }
 *            ]
 *        }
 *    }
 * ]
 * ```
 */

#ifndef MFDASM_IMPL_AST_HPP
#define MFDASM_IMPL_AST_HPP

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <shared/typedefs.hpp>

#include <mfdasm/impl/asmerror.hpp>
#include <mfdasm/impl/token.hpp>
#include <mfdasm/result.hpp>

namespace mfdasm::impl {

class ExpressionBase;
class StatementBase;

using ExpressionPtr = std::shared_ptr<ExpressionBase>;
using Expressions = std::vector<ExpressionPtr>;

using StatementPtr = std::shared_ptr<StatementBase>;
using Statements = std::vector<StatementPtr>;

struct UnresolvedIdentifier {
	std::string name;
	usize width;
	u32 lineno;
};

/**
 * @brief Used to hold information about the absolute associated addresses
 * of all identifiers alongside the current address.
 */
struct ResolvalContext {
	std::unordered_map<std::string, std::vector<u8>> identifiers;
	std::unordered_map<usize, UnresolvedIdentifier> unresolvedIdentifiers;
	usize currentAddress;
};

class ExpressionBase {
   public:
	enum Kind {
		LITERAL,
		IDENTIFIER,
		REGISTER,
		DIRECT_ADDRESS,
		INDIRECT_ADDRESS,
		STATEMENT_EXPRESSION,
	};

	virtual ~ExpressionBase() = default;

	virtual Result<std::vector<u8>, AsmError> resolveValue(
		const ResolvalContext &resolval_context) const;

	Kind kind() const;

	virtual std::string toString(u32 indent_level = 0) const;

	u32 lineno() const;

   protected:
	ExpressionBase(Kind kind, u32 lineno);

	u32 m_lineno;

   private:
	Kind m_kind;
};

class StatementBase {
   public:
	virtual ~StatementBase() = default;

	virtual Result<std::vector<u8>, AsmError> toBytes(ResolvalContext &resolval_context) const;

	virtual std::string toString(u32 indent_level = 0) const;

	const Expressions &expressions() const;

	u32 lineno() const;

   protected:
	StatementBase(Expressions expressions, u32 lineno);

	Expressions m_expressions;

	u32 m_lineno;
};

class Literal : public ExpressionBase {
   public:
	Literal(std::vector<u8> value, u32 lineno);

	Result<std::vector<u8>, AsmError> resolveValue(
		const ResolvalContext &resolval_context) const override;

	std::string toString(u32 indent_level = 0) const override;

   private:
	std::vector<u8> m_value;
};

class Identifier : public ExpressionBase {
   public:
	enum Kind {
		LABEL,
		SECTION,
		HERE,
	};

	Identifier(Kind kind, std::string name, u32 lineno);

	Result<std::vector<u8>, AsmError> resolveValue(
		const ResolvalContext &resolval_context) const override;

	Kind kind() const;

	std::string name() const;

	std::string toString(u32 indent_level = 0) const override;

   private:
	Kind m_kind;

	std::string m_name;
};

class DirectAddress : public ExpressionBase {
   public:
	enum Kind {
		MEMORY,
		REGISTER,
	};

	DirectAddress(Kind kind, ExpressionPtr value_expression, u32 lineno);

	Result<std::vector<u8>, AsmError> resolveValue(
		const ResolvalContext &resolval_context) const override;

	Kind kind() const;

	std::string toString(u32 indent_level = 0) const override;

   private:
	Kind m_kind;

	ExpressionPtr m_valueExpression;
};

class IndirectAddress : public ExpressionBase {
   public:
	enum Kind {
		MEMORY,
		REGISTER,
	};

	IndirectAddress(Kind kind, ExpressionPtr value_expression, u32 lineno);

	Result<std::vector<u8>, AsmError> resolveValue(
		const ResolvalContext &resolval_context) const override;

	Kind kind() const;

	std::string toString(u32 indent_level = 0) const override;

   private:
	Kind m_kind;

	ExpressionPtr m_valueExpression;
};

class Register : public ExpressionBase {
   public:
	enum Kind {
		AL = 0x00,
		AH = 0x01,
		ACL = 0x02,
		BL = 0x03,
		BH = 0x04,
		BCL = 0x05,
		CL = 0x06,
		CH = 0x07,
		CCL = 0x08,
		DL = 0x09,
		DH = 0x0a,
		DCL = 0x0b,
		SP = 0x0c,
		IP = 0x0d,
		AR = 0x0e,
		FL = 0x0f,
		IID = 0x10,
	};

	Register(Kind kind, u32 lineno);

	Result<std::vector<u8>, AsmError> resolveValue(
		const ResolvalContext &resolval_context) const override;

	static std::optional<Register> fromToken(const Token &token);

	Kind kind() const;

	std::string toString(u32 indent_level = 0) const override;

   private:
	Kind m_kind;
};

class Instruction : public StatementBase {
   public:
	enum Kind : u8 {
		ADC = 0x00,
		ADD = 0x01,
		AND = 0x02,
		BIN = 0x03,
		BOT = 0x04,
		CALL = 0x05,
		_RESERVED_00 = 0x06,
		CMP = 0x07,
		DEC = 0x08,
		DIV = 0x09,
		IDIV = 0x0a,
		IMUL = 0x0b,
		IN = 0x0c,
		INC = 0x0d,
		INT = 0x0e,
		IRET = 0x0f,
		JMP = 0x10,
		JZ = 0x11,
		JG = 0x12,
		JGE = 0x13,
		JL = 0x14,
		JLE = 0x15,
		JC = 0x16,
		JS = 0x17,
		JNZ = 0x18,
		JNC = 0x19,
		JNS = 0x1a,
		LD = 0x1b,
		MOV = 0x1c,
		MUL = 0x1d,
		NEG = 0x1e,
		NOP = 0x1f,
		NOT = 0x20,
		OR = 0x21,
		OUT = 0x22,
		POP = 0x23,
		PUSH = 0x24,
		RET = 0x25,
		ROL = 0x26,
		ROR = 0x27,
		SL = 0x28,
		SR = 0x29,
		ST = 0x2a,
		CLO = 0x2b,
		CLC = 0x2c,
		CLZ = 0x2d,
		CLN = 0x2e,
		CLI = 0x2f,
		_RESERVED_01 = 0x30,
		_RESERVED_02 = 0x31,
		_RESERVED_03 = 0x32,
		_RESERVED_04 = 0x33,
		_RESERVED_05 = 0x34,
		_RESERVED_06 = 0x35,
		_RESERVED_07 = 0x36,
		_RESERVED_08 = 0x37,
		_RESERVED_09 = 0x38,
		_RESERVED_10 = 0x39,
		_RESERVED_11 = 0x3a,
		STO = 0x3b,
		STC = 0x3c,
		STZ = 0x3d,
		STN = 0x3e,
		STI = 0x3f,
		_RESERVED_12 = 0x40,
		_RESERVED_13 = 0x41,
		_RESERVED_14 = 0x42,
		_RESERVED_15 = 0x43,
		_RESERVED_16 = 0x44,
		_RESERVED_17 = 0x45,
		_RESERVED_18 = 0x46,
		_RESERVED_19 = 0x47,
		_RESERVED_20 = 0x48,
		_RESERVED_21 = 0x49,
		_RESERVED_22 = 0x4a,
		SUB = 0x4b,
		TEST = 0x4c,
		XOR = 0x4d,
	};

	static bool isReserved(Kind kind);

	static std::optional<Kind> kindFromString(const std::string &str);

	Instruction(Kind kind, Expressions expressions, u32 lineno);

	Result<std::vector<u8>, AsmError> toBytes(ResolvalContext &resolval_context) const override;

	std::string toString(u32 indent_level = 0) const override;

   private:
	Kind m_kind;
};

class Directive : public StatementBase {
   public:
	enum Kind {
		DB,
		DW,
		DD,
		DS,
		TIMES,
		DEFINE,
	};

	static std::optional<Kind> kindFromToken(Token::Type type);

	Directive(Kind kind, Expressions expressions, u32 lineno);

	Result<std::vector<u8>, AsmError> toBytes(ResolvalContext &resolval_context) const override;

	std::string toString(u32 indent_level = 0) const override;

	void addExpression(ExpressionPtr expression);

   private:
	Result<std::vector<u8>, AsmError> handleDefineNumberLiteral(
		ResolvalContext &resolval_context,
		usize width) const;

	Kind m_kind;
};

class Statement : public StatementBase {
   public:
	enum Kind {
		SECTION,
		ADDRESSING_ABSOLUTE,
		ADDRESSING_RELATIVE,
		INSTRUCTION,
		DIRECTIVE,
		LABEL,
	};

	/**
	 * @brief Construct a new statement.
	 * @param kind The kind of statement.
	 * @param expressions The expressions needed for that statement.
	 * @param lineno The line number the statement was made on.
	 * @param statement The concrete statement.
	 * @note While the statement parameter is allowed to be nullptr some of the time, it is a fatal
	 * error to not set it if kind is either INSTRUCTION or DIRECTIVE. If this is not done, the
	 * constructor whil cause the program to shared::panic.
	 */
	Statement(
		Kind kind,
		Expressions expressions,
		u32 lineno,
		std::optional<StatementPtr> statement = std::nullopt);

	~Statement() = default;

	Statement(const Statement &x);

	Statement(Statement &&x);

	Kind kind() const;

	Result<std::vector<u8>, AsmError> toBytes(ResolvalContext &resolval_context) const override;

	std::string toString(u32 indent_level = 0) const override;

	std::optional<StatementPtr> maybeSubStatement() const;

   private:
	Kind m_kind;

	std::optional<StatementPtr> m_subStatement;
};

class StatementExpression : public ExpressionBase {
   public:
	StatementExpression(Statement statement, u32 lineno);

	Statement statement();

	std::string toString(u32 indent_level = 0) const override;

   private:
	Statement m_statement;
};
}  // namespace mfdasm::impl

#endif
