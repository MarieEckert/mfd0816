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

#include <algorithm>
#include <unordered_map>
#include <vector>

#include <mfdasm/impl/instruction_operand.hpp>

namespace mfdasm::impl {

/** @brief this is hell */
const std::unordered_map<Instruction::Kind, std::vector<InstructionOperand>>
	InstructionOperand::m_instructionOperandsMap = {
		{Instruction::ADC,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::ADD,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::AND,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::BIN,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::BOT,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::CALL,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::CMP,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::DEC,
		 {
			 InstructionOperand({
				 InstructionOperand::REGISTER_IMMEDIATE,
			 }),
		 }},
		{Instruction::DIV,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::IDIV,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::IMUL,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::IN,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
			 InstructionOperand({
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::INC,
		 {
			 InstructionOperand({
				 InstructionOperand::REGISTER_IMMEDIATE,
			 }),
		 }},
		{Instruction::INT,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
			 }),
		 }},
		{Instruction::IRET, {}},
		{Instruction::JMP,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::JZ,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::JG,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::JGE,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::JL,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::JLE,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::JC,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::JS,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::JNZ,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::JNC,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::JNS,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::LD,
		 {
			 InstructionOperand({
				 InstructionOperand::REGISTER_IMMEDIATE,
			 }),
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::MOV,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
			 }),
			 InstructionOperand({
				 InstructionOperand::REGISTER_IMMEDIATE,
			 }),
		 }},
		{Instruction::MUL,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::NEG,
		 {
			 InstructionOperand({
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::NOP, {}},
		{Instruction::NOT,
		 {
			 InstructionOperand({
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::OR,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::OUT,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::POP,
		 {
			 InstructionOperand({
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::PUSH,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::RET, {}},
		{Instruction::ROL,
		 {
			 InstructionOperand({
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::ROR,
		 {
			 InstructionOperand({
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::SL,
		 {
			 InstructionOperand({
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::SR,
		 {
			 InstructionOperand({
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::ST,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
			 InstructionOperand({
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::CLO, {}},
		{Instruction::CLC, {}},
		{Instruction::CLZ, {}},
		{Instruction::CLN, {}},
		{Instruction::CLI, {}},
		{Instruction::STO, {}},
		{Instruction::STC, {}},
		{Instruction::STZ, {}},
		{Instruction::STN, {}},
		{Instruction::STI, {}},
		{Instruction::SUB,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::TEST,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
		{Instruction::XOR,
		 {
			 InstructionOperand({
				 InstructionOperand::IMMEDIATE,
				 InstructionOperand::REGISTER_IMMEDIATE,
				 InstructionOperand::DIRECT,
				 InstructionOperand::REGISTER_DIRECT,
				 InstructionOperand::INDIRECT,
				 InstructionOperand::REGISTER_INDIRECT,
			 }),
		 }},
};

InstructionOperand::InstructionOperand(std::vector<InstructionOperand::Kind> allowed_kinds)
	: m_allowedKinds(allowed_kinds) {}

bool InstructionOperand::isAllowed(Kind kind) const {
	return std::find(m_allowedKinds.cbegin(), m_allowedKinds.cend(), kind) != m_allowedKinds.cend();
}

std::vector<InstructionOperand> InstructionOperand::operandsFor(Instruction::Kind instruction) {
	const auto res = m_instructionOperandsMap.find(instruction);

	return res != m_instructionOperandsMap.cend() ? res->second : std::vector<InstructionOperand>{};
}
}  // namespace mfdasm::impl
