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

#include <array>
#include <cstdint>
#include <memory>

#include <shared/log.hpp>
#include <shared/panic.hpp>

#include <mfdemu/impl/bus_device.hpp>
#include <mfdemu/impl/cpu.hpp>
#include <mfdemu/impl/instructions.hpp>

namespace mfdemu::impl {

/** class Cpu **/

void Cpu::connectAddressDevice(std::shared_ptr<BaseBusDevice> device) {
	m_addressDevice = device;
}

void Cpu::connectIoDevice(std::shared_ptr<BaseBusDevice> device) {
	m_ioDevice = device;
}

void Cpu::iclck() {
	logDebug() << "IP = 0x" << std::hex << m_regIP << std::dec << "\n";

	if(reset) {
		while(!m_state.empty()) {
			m_state.pop();
		}

		m_state.push(CpuState::INST_FETCH);
		m_state.push(CpuState::RESET);
		m_stateStep = 0;
	}

	switch(m_state.top()) {
	case CpuState::ABUS_READ_INDIRECT:
	case CpuState::ABUS_READ:
		this->abusRead();
		break;
	case CpuState::ABUS_WRITE:
		this->abusWrite();
		break;
	case CpuState::GIO_READ:
		this->gioRead();
		break;
	case CpuState::GIO_WRITE:
		this->gioWrite();
		break;
	case CpuState::INST_FETCH:
		this->fetchInst();
		break;
	case CpuState::INST_EXEC:
		this->execInst();
		break;
	case CpuState::RESET:
		this->execReset();
		break;
	}
}

void Cpu::abusRead() {
	if(m_addressDevice == nullptr) {
		shared::panic("m_addressDevice == nullptr");
	}

	switch(m_stateStep) {
	case 0: /* T0: priming pulse, AMS high */
		m_addressDevice->mode = true;
		m_addressDevice->clck();
		m_stateStep = 1;
		break;
	case 1: /* T1: address out, AMS high */
		m_addressDevice->mode = true;
		m_addressDevice->io = m_addressBusAddress;
		m_addressDevice->clck();
		m_stateStep = 2;
		break;
	case 2: /* T2: mode pulse, AMS low */
		m_addressDevice->mode = false;
		m_addressDevice->clck();
		m_stateStep = 3;
		break;
	case 3: /* T3: read pulse, AMS Low */
		m_addressDevice->mode = false;
		m_addressDevice->clck();
		m_addressBusInput = m_addressDevice->io;

		const CpuState old_state = m_state.top();
		finishState();
		if(old_state == CpuState::ABUS_READ_INDIRECT) {
			m_addressBusAddress = m_addressBusInput;
			newState(CpuState::ABUS_READ);
		}
		break;
	}
}

void Cpu::abusWrite() {
	if(m_addressDevice == nullptr) {
		shared::panic("m_addressDevice == nullptr");
	}

	switch(m_stateStep) {
	case 0: /* T0: priming pulse, AMS high */
		m_addressDevice->mode = true;
		m_addressDevice->clck();
		m_stateStep = 1;
		break;
	case 1: /* T1: address out, AMS high */
		m_addressDevice->mode = true;
		m_addressDevice->io = m_addressBusAddress;
		m_addressDevice->clck();
		m_stateStep = 2;
		break;
	case 2: /* T2: mode pulse, AMS high */
		m_addressDevice->mode = true;
		m_addressDevice->clck();
		m_stateStep = 3;
		break;
	case 3: /* T3: write pulse, AMS Low */
		m_addressDevice->mode = false;
		m_addressDevice->io = m_addressBusOutput;
		m_addressDevice->clck();

		finishState();
		break;
	}
}

void Cpu::gioRead() {
	if(m_ioDevice == nullptr) {
		shared::panic("m_ioDevice == nullptr");
	}

	switch(m_stateStep) {
	case 0: /* T0: Priming pulse, GMS high */
		m_ioDevice->mode = true;
		m_ioDevice->clck();

		m_stateStep = 1;
		break;
	case 1: /* T1: Address High-Byte pulse, GMS high */
		m_ioDevice->mode = true;
		m_ioDevice->io = (m_ioBusAddress >> 8) & 0xFF;
		m_ioDevice->clck();

		m_stateStep = 2;
		break;
	case 2: /* T2: Address Low-Byte pulse, GMS low */
		m_ioDevice->mode = false;
		m_ioDevice->io = m_ioBusAddress & 0xFF;
		m_ioDevice->clck();

		m_stateStep = 3;
		break;
	case 3: /* T3: Data High-Byte read pulse */
		m_ioDevice->clck();
		m_ioBusInput = m_ioDevice->io & 0xFF00;

		m_stateStep = 4;
		break;
	case 4: /* T4: Data Low-Byte read pulse */
		m_ioDevice->clck();
		m_ioBusInput |= m_ioDevice->io & 0xFF;

		finishState();
		break;
	}
}

void Cpu::gioWrite() {
	if(m_ioDevice == nullptr) {
		shared::panic("m_ioDevice == nullptr");
	}

	switch(m_stateStep) {
	case 0: /* T0: Priming pulse, GMS high */
		m_ioDevice->mode = true;
		m_ioDevice->clck();

		m_stateStep = 1;
		break;
	case 1: /* T1: Address High-Byte pulse, GMS high */
		m_ioDevice->mode = true;
		m_ioDevice->io = (m_ioBusAddress >> 8) & 0xFF;
		m_ioDevice->clck();

		m_stateStep = 2;
		break;
	case 2: /* T2: Address Low-Byte pulse, GMS high */
		m_ioDevice->mode = true;
		m_ioDevice->io = m_ioBusAddress & 0xFF;
		m_ioDevice->clck();

		m_stateStep = 3;
		break;
	case 3: /* T3: Data High-Byte write pulse */
		m_ioDevice->io = (m_ioBusOutput >> 8) & 0xFF;
		m_ioDevice->clck();

		m_stateStep = 4;
		break;
	case 4: /* T4: Data Low-Byte write pulse */
		m_ioDevice->io = m_ioBusOutput & 0xFF;
		m_ioDevice->clck();

		finishState();
		break;
	}
}

static std::array<u8, 0x4e> INSTRUCTION_OPERAND_COUNT = {
	/* 0x00: ADC .........*/ 1,
	/* 0x01: ADD .........*/ 1,
	/* 0x02: AND .........*/ 1,
	/* 0x03: BIN .........*/ 2,
	/* 0x04: BOT .........*/ 2,
	/* 0x05: CALL ........*/ 1,
	/* 0x06: _RESERVED_00 */ 0,
	/* 0x07: CMP .........*/ 2,
	/* 0x08: DEC .........*/ 1,
	/* 0x09: DIV .........*/ 1,
	/* 0x0a: IDIV ........*/ 1,
	/* 0x0b: IMUL ........*/ 1,
	/* 0x0c: IN ..........*/ 2,
	/* 0x0d: INC .........*/ 1,
	/* 0x0e: INT .........*/ 1,
	/* 0x0f: IRET ........*/ 0,
	/* 0x10: JMP .........*/ 1,
	/* 0x11: JZ ..........*/ 1,
	/* 0x12: JG ..........*/ 1,
	/* 0x13: JGE .........*/ 1,
	/* 0x14: JL ..........*/ 1,
	/* 0x15: JLE .........*/ 1,
	/* 0x16: JC ..........*/ 1,
	/* 0x17: JS ..........*/ 1,
	/* 0x18: JNZ .........*/ 1,
	/* 0x19: JNC .........*/ 1,
	/* 0x1a: JNS .........*/ 1,
	/* 0x1b: LD ..........*/ 2,
	/* 0x1c: MOV .........*/ 2,
	/* 0x1d: MUL .........*/ 1,
	/* 0x1e: NEG .........*/ 1,
	/* 0x1f: NOP .........*/ 0,
	/* 0x20: NOT .........*/ 1,
	/* 0x21: OR ..........*/ 1,
	/* 0x22: OUT .........*/ 2,
	/* 0x23: POP .........*/ 1,
	/* 0x24: PUSH ........*/ 1,
	/* 0x25: RET .........*/ 0,
	/* 0x26: ROL .........*/ 2,
	/* 0x27: ROR .........*/ 2,
	/* 0x28: SL ..........*/ 2,
	/* 0x29: SR ..........*/ 2,
	/* 0x2a: ST ..........*/ 2,
	/* 0x2b: CLO .........*/ 0,
	/* 0x2c: CLC .........*/ 0,
	/* 0x2d: CLZ .........*/ 0,
	/* 0x2e: CLN .........*/ 0,
	/* 0x2f: CLI .........*/ 0,
	/* 0x30: _RESERVED_01 */ 0,
	/* 0x31: _RESERVED_02 */ 0,
	/* 0x32: _RESERVED_03 */ 0,
	/* 0x33: _RESERVED_04 */ 0,
	/* 0x34: _RESERVED_05 */ 0,
	/* 0x35: _RESERVED_06 */ 0,
	/* 0x36: _RESERVED_07 */ 0,
	/* 0x37: _RESERVED_08 */ 0,
	/* 0x38: _RESERVED_09 */ 0,
	/* 0x39: _RESERVED_10 */ 0,
	/* 0x3a: _RESERVED_11 */ 0,
	/* 0x3b: STO .........*/ 0,
	/* 0x3c: STC .........*/ 0,
	/* 0x3d: STZ .........*/ 0,
	/* 0x3e: STN .........*/ 0,
	/* 0x3f: STI .........*/ 0,
	/* 0x40: _RESERVED_12 */ 0,
	/* 0x41: _RESERVED_13 */ 0,
	/* 0x42: _RESERVED_14 */ 0,
	/* 0x43: _RESERVED_15 */ 0,
	/* 0x44: _RESERVED_16 */ 0,
	/* 0x45: _RESERVED_17 */ 0,
	/* 0x46: _RESERVED_18 */ 0,
	/* 0x47: _RESERVED_19 */ 0,
	/* 0x48: _RESERVED_20 */ 0,
	/* 0x49: _RESERVED_21 */ 0,
	/* 0x4a: _RESERVED_22 */ 0,
	/* 0x4b: SUB .........*/ 1,
	/* 0x4c: TEST ........*/ 1,
	/* 0x4d: XOR .........*/ 1,
};

void Cpu::fetchInst() {
	switch(m_stateStep) {
	case 0:
		m_addressBusAddress = m_regIP;
		m_stateStep = 1;
		newState(CpuState::ABUS_READ);
		break;
	case 1: { /* Determine instruction length, decode operands. */
		m_instruction = (m_addressBusInput >> 8) & 0xFF;

		auto transform_operand_bits = [](u8 bits) -> AddressingMode {
			return {
				.immediate = bits == 0 || bits == 0b1000,
				.direct = (bits & 0b1) > 0,
				.indirect = (bits & 0b10) > 0,
				.is_register = (bits & 0b1000) > 0,
				.relative = (bits & 0b100) > 0,
			};
		};

		const u8 operand_count = INSTRUCTION_OPERAND_COUNT[m_instruction];
		if(operand_count == 0) {
#ifdef PRINT_FETCHED_INSTRUCTION
			printFetchedInstruction();
#endif
			m_stateStep = 0;
			newState(CpuState::INST_EXEC);
			break;
		}

		m_operand1 = {
			.mode = transform_operand_bits((m_addressBusInput & 0b11110000) >> 4), .value = 0};
		m_operand2 = {.mode = transform_operand_bits(m_addressBusInput & 0b1111), .value = 0};

		m_stateStep = 2;
		break;
	}
	case 2: /* Fetch operand 1 */
		m_addressBusAddress = m_regIP + 2;

		m_stateStep = 3;
		newState(CpuState::ABUS_READ);
		break;
	case 3: /* Store operand 1, Fetch operand 2 */
		if(INSTRUCTION_OPERAND_COUNT[m_instruction] == 1) {
#ifdef PRINT_FETCHED_INSTRUCTION
			printFetchedInstruction();
#endif
			m_stateStep = 0;
			newState(CpuState::INST_EXEC);
			break;
		}

		m_operand1.value = m_addressBusInput;

		m_addressBusAddress = m_regIP + 2 + (m_operand1.mode.is_register ? 1 : 2);

		m_stateStep = 4;
		newState(CpuState::ABUS_READ);
		break;
	case 4: /* Store operand 2, done */
		m_operand2.value = m_addressBusInput;

#ifdef PRINT_FETCHED_INSTRUCTION
		printFetchedInstruction();
#endif
		m_stateStep = 0;
		newState(CpuState::INST_EXEC);
		break;
	}
}

#define MAP_TO_INSTRUCTION_FUNC(name) \
	case OPCODE_##name:               \
		execInst##name();             \
		break

constexpr u8 EXEC_INST_STEP_INC_IP = 255;

void Cpu::execInst() {
	if(m_stateStep == EXEC_INST_STEP_INC_IP) {
		u8 amount = 2;
		const u8 operand_count = INSTRUCTION_OPERAND_COUNT[m_instruction];
		if(operand_count == 1) {
			amount += m_operand1.mode.is_register ? 1 : 2;
		} else {
			amount += m_operand1.mode.is_register ? 1 : 2;
			amount += m_operand2.mode.is_register ? 1 : 2;
		}

		m_regIP += amount;
		finishState();
		return;
	}

	switch(m_instruction) {
		MAP_TO_INSTRUCTION_FUNC(ADC);
		MAP_TO_INSTRUCTION_FUNC(ADD);
		MAP_TO_INSTRUCTION_FUNC(AND);
		MAP_TO_INSTRUCTION_FUNC(BIN);
		MAP_TO_INSTRUCTION_FUNC(BOT);
		MAP_TO_INSTRUCTION_FUNC(CALL);
		MAP_TO_INSTRUCTION_FUNC(CMP);
		MAP_TO_INSTRUCTION_FUNC(DEC);
		MAP_TO_INSTRUCTION_FUNC(DIV);
		MAP_TO_INSTRUCTION_FUNC(IDIV);
		MAP_TO_INSTRUCTION_FUNC(IMUL);
		MAP_TO_INSTRUCTION_FUNC(IN);
		MAP_TO_INSTRUCTION_FUNC(INC);
		MAP_TO_INSTRUCTION_FUNC(INT);
		MAP_TO_INSTRUCTION_FUNC(IRET);
		MAP_TO_INSTRUCTION_FUNC(JMP);
		MAP_TO_INSTRUCTION_FUNC(JZ);
		MAP_TO_INSTRUCTION_FUNC(JG);
		MAP_TO_INSTRUCTION_FUNC(JGE);
		MAP_TO_INSTRUCTION_FUNC(JL);
		MAP_TO_INSTRUCTION_FUNC(JLE);
		MAP_TO_INSTRUCTION_FUNC(JC);
		MAP_TO_INSTRUCTION_FUNC(JS);
		MAP_TO_INSTRUCTION_FUNC(JNZ);
		MAP_TO_INSTRUCTION_FUNC(JNC);
		MAP_TO_INSTRUCTION_FUNC(JNS);
		MAP_TO_INSTRUCTION_FUNC(LD);
		MAP_TO_INSTRUCTION_FUNC(MOV);
		MAP_TO_INSTRUCTION_FUNC(MUL);
		MAP_TO_INSTRUCTION_FUNC(NEG);
		MAP_TO_INSTRUCTION_FUNC(NOP);
		MAP_TO_INSTRUCTION_FUNC(NOT);
		MAP_TO_INSTRUCTION_FUNC(OR);
		MAP_TO_INSTRUCTION_FUNC(OUT);
		MAP_TO_INSTRUCTION_FUNC(POP);
		MAP_TO_INSTRUCTION_FUNC(PUSH);
		MAP_TO_INSTRUCTION_FUNC(RET);
		MAP_TO_INSTRUCTION_FUNC(ROL);
		MAP_TO_INSTRUCTION_FUNC(ROR);
		MAP_TO_INSTRUCTION_FUNC(SL);
		MAP_TO_INSTRUCTION_FUNC(SR);
		MAP_TO_INSTRUCTION_FUNC(ST);
		MAP_TO_INSTRUCTION_FUNC(CLO);
		MAP_TO_INSTRUCTION_FUNC(CLC);
		MAP_TO_INSTRUCTION_FUNC(CLZ);
		MAP_TO_INSTRUCTION_FUNC(CLN);
		MAP_TO_INSTRUCTION_FUNC(CLI);
		MAP_TO_INSTRUCTION_FUNC(STO);
		MAP_TO_INSTRUCTION_FUNC(STC);
		MAP_TO_INSTRUCTION_FUNC(STZ);
		MAP_TO_INSTRUCTION_FUNC(STN);
		MAP_TO_INSTRUCTION_FUNC(STI);
		MAP_TO_INSTRUCTION_FUNC(SUB);
		MAP_TO_INSTRUCTION_FUNC(TEST);
		MAP_TO_INSTRUCTION_FUNC(XOR);
	default:
		logError() << "illegal instruction!\n";
		finishState();
		break;
	}
}

void Cpu::execReset() {
	switch(m_stateStep) {
	case 0:
		m_stateStep = 1;

		m_regFL.rt = true;

		m_addressBusAddress = RESET_VECTOR;
		newState(CpuState::ABUS_READ);
		break;
	case 1:
		m_regIP = m_addressBusInput;

		m_regFL = {
			.of = false,
			.cf = false,
			.zf = false,
			.nf = false,
			.ie = false,
			.rt = false,
		};

		logDebug() << "\nreset, IP = " << std::hex << m_regIP << std::dec << "\n";

		finishState();
		break;
	}
}

#define CHECK_BIT(value, bit) ((value & static_cast<u64>(bit)) != 0)

#define SET_LOW(dest, _value) dest = (dest & 0xFF00) | (value & 0x00FF)
#define SET_HIGH(dest, value) dest = (dest & 0x00FF) | (value & 0xFF00)

void Cpu::setRegister(u8 target, u16 value) {
	switch(target) {
	case REGISTER_AL:
		SET_LOW(m_regACL, value);
		break;
	case REGISTER_BL:
		SET_LOW(m_regBCL, value);
		break;
	case REGISTER_CL:
		SET_LOW(m_regCCL, value);
		break;
	case REGISTER_DL:
		SET_LOW(m_regDCL, value);
		break;
	case REGISTER_AH:
		SET_HIGH(m_regACL, value);
		break;
	case REGISTER_BH:
		SET_HIGH(m_regBCL, value);
		break;
	case REGISTER_CH:
		SET_HIGH(m_regCCL, value);
		break;
	case REGISTER_DH:
		SET_HIGH(m_regDCL, value);
		break;
	case REGISTER_ACL:
		m_regACL = value;
		break;
	case REGISTER_BCL:
		m_regBCL = value;
		break;
	case REGISTER_CCL:
		m_regCCL = value;
		break;
	case REGISTER_DCL:
		m_regDCL = value;
		break;
	case REGISTER_SP:
		m_regSP = value;
		break;
	case REGISTER_IP:
		m_regIP = value;
		break;
	case REGISTER_AR:
		m_regAR = value;
		break;
	case REGISTER_FL:
		m_regFL = {CHECK_BIT(value, 1 << 15), CHECK_BIT(value, 1 << 14), CHECK_BIT(value, 1 << 13),
				   CHECK_BIT(value, 1 << 12), CHECK_BIT(value, 1 << 11), CHECK_BIT(value, 1 << 10)};
		break;
	case REGISTER_IID:
		m_regIID = value;
		break;
	default:
		shared::panic("invalid register " + std::to_string(target));
	}
}

#define GET_LOW(value) value & 0x00FF
#define GET_HIGH(value) value & 0xFF00

u16 Cpu::getRegister(u8 source) {
	switch(source) {
	case REGISTER_AL:
		return GET_LOW(m_regACL);
	case REGISTER_BL:
		return GET_LOW(m_regBCL);
	case REGISTER_CL:
		return GET_LOW(m_regCCL);
	case REGISTER_DL:
		return GET_LOW(m_regDCL);
	case REGISTER_AH:
		return GET_HIGH(m_regACL);
	case REGISTER_BH:
		return GET_HIGH(m_regBCL);
	case REGISTER_CH:
		return GET_HIGH(m_regCCL);
	case REGISTER_DH:
		return GET_HIGH(m_regDCL);
	case REGISTER_ACL:
		return m_regACL;
	case REGISTER_BCL:
		return m_regBCL;
	case REGISTER_CCL:
		return m_regCCL;
	case REGISTER_DCL:
		return m_regDCL;
	case REGISTER_SP:
		return m_regSP;
	case REGISTER_IP:
		return m_regIP;
	case REGISTER_AR:
		return m_regAR;
	case REGISTER_FL:
		return UINT16_MAX & (m_regFL.of << 15 | m_regFL.cf << 14 | m_regFL.zf << 13 |
							 m_regFL.nf << 12 | m_regFL.ie << 11 | m_regFL.rt << 10);
	case REGISTER_IID:
		return m_regIID;
	default:
		shared::panic("invalid register " + std::to_string(source));
	}
}

/** @todo: implement */
void Cpu::execInstADC() {}

/** @todo: implement */
void Cpu::execInstADD() {}

/** @todo: implement */
void Cpu::execInstAND() {}

/** @todo: implement */
void Cpu::execInstBIN() {}

/** @todo: implement */
void Cpu::execInstBOT() {}

/** @todo: implement */
void Cpu::execInstCALL() {}

/** @todo: implement */
void Cpu::execInstCMP() {}

/** @todo: implement */
void Cpu::execInstDEC() {}

/** @todo: implement */
void Cpu::execInstDIV() {}

/** @todo: implement */
void Cpu::execInstIDIV() {}

/** @todo: implement */
void Cpu::execInstIMUL() {}

/** @todo: implement */
void Cpu::execInstIN() {}

/** @todo: implement */
void Cpu::execInstINC() {}

/** @todo: implement */
void Cpu::execInstINT() {}

/** @todo: implement */
void Cpu::execInstIRET() {}

/** @todo: implement */
void Cpu::execInstJMP() {}

/** @todo: implement */
void Cpu::execInstJZ() {}

/** @todo: implement */
void Cpu::execInstJG() {}

/** @todo: implement */
void Cpu::execInstJGE() {}

/** @todo: implement */
void Cpu::execInstJL() {}

/** @todo: implement */
void Cpu::execInstJLE() {}

/** @todo: implement */
void Cpu::execInstJC() {}

/** @todo: implement */
void Cpu::execInstJS() {}

/** @todo: implement */
void Cpu::execInstJNZ() {}

/** @todo: implement */
void Cpu::execInstJNC() {}

/** @todo: implement */
void Cpu::execInstJNS() {}

/** @todo: implement */
void Cpu::execInstLD() {}

/** @todo: implement */
void Cpu::execInstMOV() {
	logDebug() << "move\n";
	setRegister(
		(m_operand2.value & 0xFF00) >> 8,
		(m_operand1.mode.is_register ? getRegister((m_operand1.value & 0xFF00) >> 8)
									 : m_operand1.value));
	m_stateStep = EXEC_INST_STEP_INC_IP;
}

/** @todo: implement */
void Cpu::execInstMUL() {}

/** @todo: implement */
void Cpu::execInstNEG() {}

void Cpu::execInstNOP() {
	m_stateStep = EXEC_INST_STEP_INC_IP;
}

/** @todo: implement */
void Cpu::execInstNOT() {}

/** @todo: implement */
void Cpu::execInstOR() {}

/** @todo: implement */
void Cpu::execInstOUT() {}

/** @todo: implement */
void Cpu::execInstPOP() {}

/** @todo: implement */
void Cpu::execInstPUSH() {}

/** @todo: implement */
void Cpu::execInstRET() {}

/** @todo: implement */
void Cpu::execInstROL() {}

/** @todo: implement */
void Cpu::execInstROR() {}

/** @todo: implement */
void Cpu::execInstSL() {}

/** @todo: implement */
void Cpu::execInstSR() {}

void Cpu::execInstST() {
	constexpr u8 READ_VALUE = 16;
	constexpr u8 WRITE_VALUE = 32;

	switch(m_stateStep) {
	case 0:
		m_stash1 = m_operand1.mode.is_register ? this->getRegister((m_operand1.value & 0xFF00) >> 8)
											   : m_operand1.value;
		m_stash2 = m_operand2.mode.is_register ? this->getRegister((m_operand2.value & 0xFF00) >> 8)
											   : m_operand2.value;

		if(!m_operand2.mode.immediate) {
			m_addressBusAddress = m_stash2;
			m_stateStep = READ_VALUE;
			newState(m_operand2.mode.direct ? CpuState::ABUS_READ : CpuState::ABUS_READ_INDIRECT);
			break;
		}
	case READ_VALUE:
		if(!m_operand1.mode.immediate) {
			m_addressBusAddress = m_stash1;
			m_stateStep = WRITE_VALUE;
			newState(m_operand1.mode.direct ? CpuState::ABUS_READ : CpuState::ABUS_READ_INDIRECT);
			break;
		}
	case WRITE_VALUE:
		m_addressBusAddress = m_stash2;
		m_addressBusOutput = m_stash1;
		m_stateStep = EXEC_INST_STEP_INC_IP;
		newState(CpuState::ABUS_WRITE);
		break;
	}
}

void Cpu::execInstCLO() {
	m_regFL.of = false;
	m_stateStep = EXEC_INST_STEP_INC_IP;
}

void Cpu::execInstCLC() {
	m_regFL.cf = false;
	m_stateStep = EXEC_INST_STEP_INC_IP;
}

void Cpu::execInstCLZ() {
	m_regFL.zf = false;
	m_stateStep = EXEC_INST_STEP_INC_IP;
}

void Cpu::execInstCLN() {
	m_regFL.nf = false;
	m_stateStep = EXEC_INST_STEP_INC_IP;
}

void Cpu::execInstCLI() {
	m_regFL.ie = false;
	m_stateStep = EXEC_INST_STEP_INC_IP;
}

void Cpu::execInstSTO() {
	m_regFL.of = true;
	m_stateStep = EXEC_INST_STEP_INC_IP;
}

void Cpu::execInstSTC() {
	m_regFL.cf = true;
	m_stateStep = EXEC_INST_STEP_INC_IP;
}

void Cpu::execInstSTZ() {
	m_regFL.zf = true;
	m_stateStep = EXEC_INST_STEP_INC_IP;
}

void Cpu::execInstSTN() {
	m_regFL.nf = true;
	m_stateStep = EXEC_INST_STEP_INC_IP;
}

void Cpu::execInstSTI() {
	m_regFL.ie = true;
	m_stateStep = EXEC_INST_STEP_INC_IP;
}

/** @todo: implement */
void Cpu::execInstSUB() {}

/** @todo: implement */
void Cpu::execInstTEST() {}

/** @todo: implement */
void Cpu::execInstXOR() {}

void Cpu::newState(CpuState state) {
	m_state.push(state);
	m_stepStash.push(m_stateStep);
	m_stateStep = 0;
}

void Cpu::finishState() {
	if(m_stepStash.empty()) {
		m_stateStep = 0;

		while(!m_state.empty()) {
			m_state.pop();
		}

		m_state.push(CpuState::INST_FETCH);
		return;
	}

	m_stateStep = m_stepStash.top();
	m_stepStash.pop();
	m_state.pop();
}

void Cpu::printFetchedInstruction() const {
	logDebug() << "fetched instruction 0x" << std::hex << static_cast<u32>(m_instruction) << "\n"
			   << "\t-> Operand 1: value = 0x" << static_cast<u32>(m_operand1.value)
			   << ", addressing mode {\n"
			   << "\t\t-> is_register: " << std::to_string(m_operand1.mode.is_register) << "\n"
			   << "\t\t-> direct: " << std::to_string(m_operand1.mode.direct) << "\n"
			   << "\t\t-> indirect: " << std::to_string(m_operand1.mode.indirect) << "\n"
			   << "\t\t-> relative: " << std::to_string(m_operand1.mode.relative) << "\n"
			   << "\t\t-> immediate: " << std::to_string(m_operand1.mode.immediate) << "\n"
			   << "\t-> Operand 2: value = 0x" << static_cast<u32>(m_operand2.value)
			   << ", addressing mode {\n"
			   << "\t\t-> is_register: " << std::to_string(m_operand2.mode.is_register) << "\n"
			   << "\t\t-> direct: " << std::to_string(m_operand2.mode.direct) << "\n"
			   << "\t\t-> indirect: " << std::to_string(m_operand2.mode.indirect) << "\n"
			   << "\t\t-> relative: " << std::to_string(m_operand2.mode.relative) << "\n"
			   << "\t\t-> immediate: " << std::to_string(m_operand2.mode.immediate) << "\n";
}

}  // namespace mfdemu::impl