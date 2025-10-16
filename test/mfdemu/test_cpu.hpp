#include <mfdemu/impl/cpu.hpp>

namespace test::mfdemu {

using namespace ::mfdemu::impl;

class CpuTest : public Cpu {
   public:
	using CpuState = Cpu::CpuState;

	inline void newState(CpuState state) { Cpu::newState(state); }
	inline CpuState state() const { return m_state.top(); }

	u16 &m_regACL = Cpu::m_regACL;
	u16 &m_regBCL = Cpu::m_regBCL;
	u16 &m_regCCL = Cpu::m_regCCL;
	u16 &m_regDCL = Cpu::m_regDCL;
	u16 &m_regSP = Cpu::m_regSP;
	u16 &m_regIP = Cpu::m_regIP;
	u16 &m_regAR = Cpu::m_regAR;
	CpuFlags &m_regFL = Cpu::m_regFL;

	u16 &m_ioBusInput = Cpu::m_ioBusInput;
	u16 &m_ioBusOutput = Cpu::m_ioBusOutput;
	u16 &m_ioBusAddress = Cpu::m_ioBusAddress;
};
}  // namespace test::mfdemu
