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

#include <shared/typedefs.hpp>

namespace mfdemu::impl {

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

   private:
	enum class CpuState : u8 {
		ABUS_READ,
		ABUS_WRITE,
		GIO_READ,
		GIO_WRITE,
		INST_EXEC,
		INST_FETCH,
	};

	void abusRead();
	void abusWrite();
	void gioRead();
	void gioWrite();
	void fetchInst();
	void execInst();

	/** internal state */
	CpuState m_state;
	u8 m_stateStep;

	u8 m_instStep;
	u16 m_instruction;
	Operand m_operand1;
	Operand m_operand2;

	/** exclusively output pins */
	bool m_pinAMS;
	bool m_pinGMS;
	bool m_pinCLK;
	bool m_pinIRA;

	/** registers **/
	u16 m_regACL;
	u16 m_regBCL;
	u16 m_regCCL;
	u16 m_regDCL;
	u16 m_regSP;
	u16 m_regIP;
	u16 m_regAR;
	CpuFlags m_regFL;
	u16 m_regIID;
};

}  // namespace mfdemu::impl

#endif
