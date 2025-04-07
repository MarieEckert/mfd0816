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

#ifndef MFDEMU_IMPL_CPU_HPP
#define MFDEMU_IMPL_CPU_HPP

#include <memory>
#include <stack>

#include <shared/typedefs.hpp>

#include <mfdemu/impl/bus_device.hpp>

namespace mfdemu::impl {

constexpr u16 RESET_VECTOR = 0xfffe;

struct CpuFlags {
	bool of;
	bool cf;
	bool zf;
	bool nf;
	bool ie;
	bool rt;
};

struct AddressingMode {
	bool immediate;
	bool direct;
	bool indirect;
	bool is_register;
	bool relative;
};

struct Operand {
	AddressingMode mode;
	u16 value;
};

class Cpu {
   public:
	void iclck();

	u16 aio;
	u8 gio;
	bool irq;
	bool reset;
	inline bool ams() const { return m_pinAMS; }
	inline bool gms() const { return m_pinGMS; }
	inline bool clk() const { return m_pinCLK; }
	inline bool ira() const { return m_pinIRA; }

	void connectAddressDevice(std::shared_ptr<BaseBusDevice> device);

	void connectIoDevice(std::shared_ptr<BaseBusDevice> device);

   private:
	enum class CpuState : u8 {
		ABUS_READ,
		ABUS_READ_INDIRECT,
		ABUS_WRITE,
		ABUS_WRITE_INDIRECT,
		GIO_READ,
		GIO_WRITE,
		INST_EXEC,
		INST_FETCH,
		RESET,
	};

	/** general operations */

	void abusRead();
	void abusWrite();
	void abusWriteIndirect();
	void gioRead();
	void gioWrite();
	void fetchInst();
	void execInst();
	void execReset();

	/** instructions */

	void execInstADC();
	void execInstADD();
	void execInstAND();
	void execInstBIN();
	void execInstBOT();
	void execInstCALL();
	void execInstCMP();
	void execInstDEC();
	void execInstDIV();
	void execInstIDIV();
	void execInstIMUL();
	void execInstIN();
	void execInstINC();
	void execInstINT();
	void execInstIRET();
	void execInstJMP();
	void execInstJZ();
	void execInstJG();
	void execInstJGE();
	void execInstJL();
	void execInstJLE();
	void execInstJC();
	void execInstJS();
	void execInstJNZ();
	void execInstJNC();
	void execInstJNS();
	void execInstLD();
	void execInstMOV();
	void execInstMUL();
	void execInstNEG();
	void execInstNOP();
	void execInstNOT();
	void execInstOR();
	void execInstOUT();
	void execInstPOP();
	void execInstPUSH();
	void execInstRET();
	void execInstROL();
	void execInstROR();
	void execInstSL();
	void execInstSR();
	void execInstST();
	void execInstCLO();
	void execInstCLC();
	void execInstCLZ();
	void execInstCLN();
	void execInstCLI();
	void execInstSTO();
	void execInstSTC();
	void execInstSTZ();
	void execInstSTN();
	void execInstSTI();
	void execInstSUB();
	void execInstTEST();
	void execInstXOR();

	/** internal state
	 *
	 * # CpuState
	 * m_state, m_stateStep and m_stepStash are used to keep track
	 * of what the Cpu is/should be currently doing.
	 * The top of m_state holds the current state the Cpu is in whilst
	 * the top of m_stepStash stores the progress of the last state the
	 * cpu was in.
	 *
	 * # Instruction
	 * TODO: not documented yet
	 *
	 * # Address & IO Bus
	 * m_addressBusInput/m_ioBusInput is used to temporarily store data read from the
	 * address/io bus before assigning it to the correct register.
	 * m_addressBusOutput/m_ioBusOutput is used to store the data which should be written
	 * to the bus.
	 * m_addressBusAddress/m_ioBusAddress is used to store the target address.
	 */

	/**
	 * @brief Go to a new state in execution. This pushes
	 * the given CpuState to m_state and saves the old state
	 * step progress (m_stateStep) to m_stepStash so that it
	 * can be recovered after the state finishes.
	 */
	void newState(CpuState state);

	/**
	 * @brief Go to the previous state of the Cpu and restore
	 * the value of m_stateStep.
	 *
	 * Pops the top values of m_stepStash and m_state.
	 */
	void finishState();

	std::stack<CpuState, std::vector<CpuState>> m_state;
	std::stack<u8> m_stepStash;
	u8 m_stateStep{0};

	u16 m_instruction{0};
	Operand m_operand1;
	Operand m_operand2;

	u16 m_addressBusInput{0};
	u16 m_addressBusOutput{0};
	u16 m_addressBusAddress{0};

	u16 m_ioBusInput{0};
	u16 m_ioBusOutput{0};
	u16 m_ioBusAddress{0};

	/* Internal stashing registers */
	u16 m_stash1{0};
	u16 m_stash2{0};
	u16 m_stash3{0};
	u16 m_stash4{0};

	/** exclusively output pins */
	bool m_pinAMS{false};
	bool m_pinGMS{false};
	bool m_pinCLK{false};
	bool m_pinIRA{false};

	/** registers **/
	u16 m_regACL{0};
	u16 m_regBCL{0};
	u16 m_regCCL{0};
	u16 m_regDCL{0};
	u16 m_regSP{0};
	u16 m_regIP{0};
	u16 m_regAR{0};
	CpuFlags m_regFL{0, 0, 0, 0, 0, 0};
	u16 m_regIID{0};

	void setRegister(u8 target, u16 value);
	u16 getRegister(u8 source);

	/** connected devices (for impl.) */
	std::shared_ptr<BaseBusDevice> m_addressDevice;
	std::shared_ptr<BaseBusDevice> m_ioDevice;

	/** debug utils */
	void printFetchedInstruction() const;
};

}  // namespace mfdemu::impl

#endif
