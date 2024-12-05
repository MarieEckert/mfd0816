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

#include <mfdasm/typedefs.hpp>

namespace mfdasm::impl {

/**
 * @brief Used to hold information about the absolute associated addresses
 * of all identifiers alongside the current address.
 */
struct ResolvalContext {
	std::unordered_map<std::string, std::vector<u8>> identifiers;
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
	};

	~ExpressionBase() = default;

	std::optional<std::vector<u8>> resolveValue(const ResolvalContext &resolval_context) const;

	Kind kind() const;

   protected:
	Kind m_kind;
};

class StatementBase {
   public:
	~StatementBase() = default;

	std::vector<u8> toBytes(ResolvalContext &resolval_context) const;

	std::string toString() const;
};

class Literal : public ExpressionBase {
   public:
	Literal(std::vector<u8> value);

	std::optional<std::vector<u8>> resolveValue(const ResolvalContext &resolval_context) const;

   private:
	std::vector<u8> m_value;
};

class Identifier : public ExpressionBase {
   public:
	enum Kind {
		LABEL,
		SECTION,
	};

	Identifier(Kind kind, std::string name);

	std::optional<std::vector<u8>> resolveValue(const ResolvalContext &resolval_context) const;

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

	DirectAddress(Kind kind, std::vector<u8> value);

	Kind kind() const;

   private:
	Kind m_kind;

	std::vector<u8> m_value;
};

class IndirectAddress : public ExpressionBase {
   public:
	enum Kind {
		MEMORY,
		REGISTER,
	};

	IndirectAddress(Kind kind, std::vector<u8> value);

	Kind kind() const;

   private:
	Kind m_kind;

	std::vector<u8> m_value;
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

	Instruction(Kind kind, std::vector<ExpressionBase> expressions);

	std::vector<u8> toBytes(ResolvalContext &resolval_context) const;

	std::string toString() const;

   private:
	Kind m_kind;

	std::vector<ExpressionBase> m_expressions;
};

class Directive : public StatementBase {
   public:
	enum Kind {
		DB,
		DW,
		DD,
		TIMES,
	};

	static std::optional<Kind> kindFromString(const std::string &str);

	std::vector<u8> toBytes(ResolvalContext &resolval_context) const;

	std::string toString() const;
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
	 * @param statement The concrete statement.
	 * @note While the statement parameter is allowed to be nullptr some of the time, it is a fatal
	 * error to not set it if kind is either INSTRUCTION or DIRECTIVE. If this is not done, the
	 * constructor whil cause the program to panic.
	 */
	Statement(
		Kind kind,
		std::vector<ExpressionBase> expressions,
		std::shared_ptr<StatementBase> statement = nullptr);

	Statement(Statement &x);

	Statement(Statement &&x);

	Kind kind() const;

	std::vector<u8> toBytes(ResolvalContext &resolval_context) const;

	std::string toString() const;

   private:
	Kind m_kind;

	std::vector<ExpressionBase> m_expressions;

	std::shared_ptr<StatementBase> m_subStatement;
};
}  // namespace mfdasm::impl

#endif