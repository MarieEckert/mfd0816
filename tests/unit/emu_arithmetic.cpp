#include <cstdint>
#include <cstdlib>
#include <memory>
#include <random>
#include <vector>

#include <shared/log.hpp>

#include <mfdemu/impl/bus/gio_device.hpp>
#include <mfdemu/impl/cpu.hpp>
#include <mfdemu/impl/instructions.hpp>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include "doctest/doctest.h"

using namespace mfdemu;
using namespace mfdemu::impl;

class CpuTest : public Cpu {
   public:
	using CpuState = Cpu::CpuState;

	void newState(CpuState state) { Cpu::newState(state); }

	CpuState state() const { return m_state.top(); }

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

class AioTestDevice : public BaseBusDevice<u16> {
   public:
	AioTestDevice() { m_data.resize(0xffff); }
	void clck() override {
		switch(m_step) {
		case 0:
			if(mode) {
				m_step = 1;
			}
			break;
		case 1:
			if(!mode) {
				m_step = 0;
				break;
			}

			m_address = io;
			m_step = 2;
			break;
		case 2:
			m_write = mode;
			m_step = 3;
			break;
		case 3:
			if(m_write) {
				if(m_address >= m_data.size() || m_address + 1 >= m_data.size()) { /* discard */
					m_step = 0;
					break;
				}

				if(m_address == 0x5000) { /* debug thingy */
					logInfo() << "write to 0x5000 , value = 0b" << std::bitset<16>(io).to_string()
							  << "\n";
				}

				m_data[m_address] = (io >> 8) & 0xFF;
				m_data[m_address + 1] = io & 0xFF;
			} else {
				io = m_address >= m_data.size() || m_address + 1 >= m_data.size()
						 ? 0
						 : (m_data[m_address] << 8) | m_data[m_address + 1];
			}

			m_step = 0;
			break;
		}
	}

	/** internal state */
	u8 m_step{0};
	u32 m_address;
	bool m_write;

	/** data */
	std::vector<u8> m_data;
};

std::shared_ptr<AioTestDevice> prepareTestDevice(const std::vector<u8> &code) {
	auto dev = std::make_shared<AioTestDevice>();
	REQUIRE(dev != nullptr);

	dev->m_data.insert(dev->m_data.cbegin(), code.cbegin(), code.cend());
	return dev;
}

void divTest(u32 dividend, u16 divisor) {
	CpuTest cpu;

	const u16 ar_expected = dividend / divisor;
	const u16 acl_expected = dividend % divisor;

	cpu.m_regACL = static_cast<u16>((dividend >> 16) & 0xFFFF);
	cpu.m_regAR = static_cast<u16>(dividend & 0xFFFF);

	cpu.connectAddressDevice(prepareTestDevice({
		OPCODE_DIV,
		0x00,
		static_cast<u8>(divisor >> 8),
		static_cast<u8>(divisor),
	}));

	cpu.newState(CpuTest::CpuState::INST_FETCH);
	while(cpu.state() == CpuTest::CpuState::INST_FETCH ||
		  cpu.state() == CpuTest::CpuState::ABUS_READ) {
		cpu.iclck();
	}

	while(cpu.state() == CpuTest::CpuState::INST_EXEC ||
		  cpu.state() == CpuTest::CpuState::ABUS_READ) {
		cpu.iclck();
	}

	REQUIRE_EQ(cpu.m_regAR, ar_expected);
	REQUIRE_EQ(cpu.m_regACL, acl_expected);
}

TEST_CASE("div") {
	divTest(100, 10);
	divTest(UINT16_MAX, 2);
	divTest(UINT32_MAX, 2);
}

void idivTest(i32 dividend, i16 divisor) {
	CpuTest cpu;

	const i16 ar_expected = dividend / divisor;
	const i16 acl_expected = dividend % divisor;

	cpu.m_regACL = static_cast<u16>((dividend >> 16) & 0xFFFF);
	cpu.m_regAR = static_cast<u16>(dividend & 0xFFFF);

	cpu.connectAddressDevice(prepareTestDevice({
		OPCODE_IDIV,
		0x00,
		static_cast<u8>((divisor >> 8) & 0xFF),
		static_cast<u8>(divisor & 0xFF),
	}));

	cpu.newState(CpuTest::CpuState::INST_FETCH);
	while(cpu.state() == CpuTest::CpuState::INST_FETCH ||
		  cpu.state() == CpuTest::CpuState::ABUS_READ) {
		cpu.iclck();
	}

	while(cpu.state() == CpuTest::CpuState::INST_EXEC ||
		  cpu.state() == CpuTest::CpuState::ABUS_READ) {
		cpu.iclck();
	}

	CHECK_EQ(static_cast<i16>(cpu.m_regAR), ar_expected);
	CHECK_EQ(static_cast<i16>(cpu.m_regACL), acl_expected);
}

TEST_CASE("idiv") {
	idivTest(100, 10);
	idivTest(-100, 10);
	idivTest(-100, -10);
	idivTest(100, -10);
	idivTest(INT16_MAX, 2);
	idivTest(INT16_MIN, 2);
	idivTest(INT16_MIN, -2);
	idivTest(INT16_MAX, -2);
	idivTest(INT32_MAX, 2);
	idivTest(INT32_MIN, 2);
	idivTest(INT32_MIN, -2);
	idivTest(INT32_MAX, -2);
}

void mulTest(u16 factor1, u16 factor2) {
	CpuTest cpu;

	const u32 product = factor1 * factor2;
	const u16 ar_expected = product & 0xFFFF;
	const u16 acl_expected = product >> 16;
	const bool of_expected = acl_expected != 0;
	const bool cf_expected = of_expected;

	cpu.m_regAR = factor1;

	cpu.connectAddressDevice(prepareTestDevice({
		OPCODE_MUL,
		0x00,
		static_cast<u8>((factor2 >> 8) & 0xFF),
		static_cast<u8>(factor2 & 0xFF),
	}));

	cpu.newState(CpuTest::CpuState::INST_FETCH);
	while(cpu.state() == CpuTest::CpuState::INST_FETCH ||
		  cpu.state() == CpuTest::CpuState::ABUS_READ) {
		cpu.iclck();
	}

	while(cpu.state() == CpuTest::CpuState::INST_EXEC ||
		  cpu.state() == CpuTest::CpuState::ABUS_READ) {
		cpu.iclck();
	}

	CHECK_EQ(static_cast<u16>(cpu.m_regAR), ar_expected);
	CHECK_EQ(static_cast<u16>(cpu.m_regACL), acl_expected);
	CHECK_EQ(cpu.m_regFL.of, of_expected);
	CHECK_EQ(cpu.m_regFL.cf, cf_expected);
}

TEST_CASE("mul") {
	mulTest(100, 10);
	mulTest(UINT16_MAX, 10);
}

void imulTest(i16 factor1, i16 factor2) {
	CpuTest cpu;

	const i32 product = factor1 * factor2;

	const i16 ar_expected = product & 0xFFFF;
	const i16 acl_expected = (product >> 16) & 0xFFFF;
	const bool of_expected = static_cast<i32>(ar_expected) != product;
	const bool cf_expected = of_expected;

	cpu.m_regAR = factor1;

	cpu.connectAddressDevice(prepareTestDevice({
		OPCODE_IMUL,
		0x00,
		static_cast<u8>((factor2 >> 8) & 0xFF),
		static_cast<u8>(factor2 & 0xFF),
	}));

	cpu.newState(CpuTest::CpuState::INST_FETCH);
	while(cpu.state() == CpuTest::CpuState::INST_FETCH ||
		  cpu.state() == CpuTest::CpuState::ABUS_READ) {
		cpu.iclck();
	}

	while(cpu.state() == CpuTest::CpuState::INST_EXEC ||
		  cpu.state() == CpuTest::CpuState::ABUS_READ) {
		cpu.iclck();
	}

	CHECK_EQ(static_cast<i16>(cpu.m_regAR), ar_expected);
	CHECK_EQ(static_cast<i16>(cpu.m_regACL), acl_expected);
	CHECK_EQ(cpu.m_regFL.of, of_expected);
	CHECK_EQ(cpu.m_regFL.cf, cf_expected);
}

TEST_CASE("imul") {
	imulTest(100, 10);
	imulTest(-100, 10);
	imulTest(-100, -10);
	imulTest(100, -10);
	imulTest(INT16_MAX, 2);
	imulTest(INT16_MIN, 2);
	imulTest(INT16_MIN, -2);
	imulTest(INT16_MAX, -2);
}