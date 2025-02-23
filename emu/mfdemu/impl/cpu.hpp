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

#include <mfdemu/impl/bus_device.hpp>
#include <shared/typedefs.hpp>

namespace mfdemu::impl {

constexpr u16 RESET_VECTOR = 0xfffe;

struct CpuFlags {
	bool of;
	bool cf;
	bool zf;
	bool nf;
	bool ie;
	bool rt;
	bool rs;
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
		ABUS_WRITE,
		GIO_READ,
		GIO_WRITE,
		INST_EXEC,
		INST_FETCH,
		RESET,
	};

	void abusRead();
	void abusWrite();
	void gioRead();
	void gioWrite();
	void fetchInst();
	void execInst();
	void execReset();

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
	 * # Address Bus
	 * m_addressBusInput is used to temporarily store data read from the
	 * address bus before assigning it to the correct register.
	 * m_addressBusOutput is used to store the data which should be written
	 * to the bus.
	 * m_addressBusAddress is used to store the target address.
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

	u8 m_instStep{0};
	u16 m_instruction{0};
	Operand m_operand1;
	Operand m_operand2;

	u16 m_addressBusInput{0};
	u16 m_addressBusOutput{0};
	u16 m_addressBusAddress{0};

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
	CpuFlags m_regFL{0, 0, 0, 0, 0, 0, 0};
	u16 m_regIID{0};

	/** connected devices (for impl.) */
	std::shared_ptr<BaseBusDevice> m_addressDevice;
	std::shared_ptr<BaseBusDevice> m_ioDevice;
};

}  // namespace mfdemu::impl

#endif
