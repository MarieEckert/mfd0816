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

#ifndef MFDEMU_IMPL_INSTRUCTIONS_HPP
#define MFDEMU_IMPL_INSTRUCTIONS_HPP

#include <shared/typedefs.hpp>

namespace mfdemu::impl {

constexpr u8 OPCODE_ADC = 0x00;
constexpr u8 OPCODE_ADD = 0x01;
constexpr u8 OPCODE_AND = 0x02;
constexpr u8 OPCODE_BIN = 0x03;
constexpr u8 OPCODE_BOT = 0x04;
constexpr u8 OPCODE_CALL = 0x05;
constexpr u8 OPCODE__RESERVED_00 = 0x06;
constexpr u8 OPCODE_CMP = 0x07;
constexpr u8 OPCODE_DEC = 0x08;
constexpr u8 OPCODE_DIV = 0x09;
constexpr u8 OPCODE_IDIV = 0x0a;
constexpr u8 OPCODE_IMUL = 0x0b;
constexpr u8 OPCODE_IN = 0x0c;
constexpr u8 OPCODE_INC = 0x0d;
constexpr u8 OPCODE_INT = 0x0e;
constexpr u8 OPCODE_IRET = 0x0f;
constexpr u8 OPCODE_JMP = 0x10;
constexpr u8 OPCODE_JZ = 0x11;
constexpr u8 OPCODE_JG = 0x12;
constexpr u8 OPCODE_JGE = 0x13;
constexpr u8 OPCODE_JL = 0x14;
constexpr u8 OPCODE_JLE = 0x15;
constexpr u8 OPCODE_JC = 0x16;
constexpr u8 OPCODE_JS = 0x17;
constexpr u8 OPCODE_JNZ = 0x18;
constexpr u8 OPCODE_JNC = 0x19;
constexpr u8 OPCODE_JNS = 0x1a;
constexpr u8 OPCODE_LD = 0x1b;
constexpr u8 OPCODE_MOV = 0x1c;
constexpr u8 OPCODE_MUL = 0x1d;
constexpr u8 OPCODE_NEG = 0x1e;
constexpr u8 OPCODE_NOP = 0x1f;
constexpr u8 OPCODE_NOT = 0x20;
constexpr u8 OPCODE_OR = 0x21;
constexpr u8 OPCODE_OUT = 0x22;
constexpr u8 OPCODE_POP = 0x23;
constexpr u8 OPCODE_PUSH = 0x24;
constexpr u8 OPCODE_RET = 0x25;
constexpr u8 OPCODE_ROL = 0x26;
constexpr u8 OPCODE_ROR = 0x27;
constexpr u8 OPCODE_SL = 0x28;
constexpr u8 OPCODE_SR = 0x29;
constexpr u8 OPCODE_ST = 0x2a;
constexpr u8 OPCODE_CLO = 0x2b;
constexpr u8 OPCODE_CLC = 0x2c;
constexpr u8 OPCODE_CLZ = 0x2d;
constexpr u8 OPCODE_CLN = 0x2e;
constexpr u8 OPCODE_CLI = 0x2f;
constexpr u8 OPCODE__RESERVED_01 = 0x30;
constexpr u8 OPCODE__RESERVED_02 = 0x31;
constexpr u8 OPCODE__RESERVED_03 = 0x32;
constexpr u8 OPCODE__RESERVED_04 = 0x33;
constexpr u8 OPCODE__RESERVED_05 = 0x34;
constexpr u8 OPCODE__RESERVED_06 = 0x35;
constexpr u8 OPCODE__RESERVED_07 = 0x36;
constexpr u8 OPCODE__RESERVED_08 = 0x37;
constexpr u8 OPCODE__RESERVED_09 = 0x38;
constexpr u8 OPCODE__RESERVED_10 = 0x39;
constexpr u8 OPCODE__RESERVED_11 = 0x3a;
constexpr u8 OPCODE_STO = 0x3b;
constexpr u8 OPCODE_STC = 0x3c;
constexpr u8 OPCODE_STZ = 0x3d;
constexpr u8 OPCODE_STN = 0x3e;
constexpr u8 OPCODE_STI = 0x3f;
constexpr u8 OPCODE__RESERVED_12 = 0x40;
constexpr u8 OPCODE__RESERVED_13 = 0x41;
constexpr u8 OPCODE__RESERVED_14 = 0x42;
constexpr u8 OPCODE__RESERVED_15 = 0x43;
constexpr u8 OPCODE__RESERVED_16 = 0x44;
constexpr u8 OPCODE__RESERVED_17 = 0x45;
constexpr u8 OPCODE__RESERVED_18 = 0x46;
constexpr u8 OPCODE__RESERVED_19 = 0x47;
constexpr u8 OPCODE__RESERVED_20 = 0x48;
constexpr u8 OPCODE__RESERVED_21 = 0x49;
constexpr u8 OPCODE__RESERVED_22 = 0x4a;
constexpr u8 OPCODE_SUB = 0x4b;
constexpr u8 OPCODE_TEST = 0x4c;
constexpr u8 OPCODE_XOR = 0x4d;

/** registers */

constexpr u8 REGISTER_AL = 0x00;
constexpr u8 REGISTER_AH = 0x01;
constexpr u8 REGISTER_ACL = 0x02;
constexpr u8 REGISTER_BL = 0x03;
constexpr u8 REGISTER_BH = 0x04;
constexpr u8 REGISTER_BCL = 0x05;
constexpr u8 REGISTER_CL = 0x06;
constexpr u8 REGISTER_CH = 0x07;
constexpr u8 REGISTER_CCL = 0x08;
constexpr u8 REGISTER_DL = 0x09;
constexpr u8 REGISTER_DH = 0x0a;
constexpr u8 REGISTER_DCL = 0x0b;
constexpr u8 REGISTER_SP = 0x0c;
constexpr u8 REGISTER_IP = 0x0d;
constexpr u8 REGISTER_AR = 0x0e;
constexpr u8 REGISTER_FL = 0x0f;
constexpr u8 REGISTER_IID = 0x10;

}  // namespace mfdemu::impl

#endif