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

template <typename IntType>
IntType random() {
	std::random_device rd;
	auto seed_data = std::array<int, std::mt19937::state_size>{};
	std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
	std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
	std::mt19937 generator(seq);
	std::uniform_int_distribution<IntType> dis(
		std::numeric_limits<IntType>::min(), std::numeric_limits<IntType>::max());
	return dis(generator);
}

TEST_CASE("div") {
	CpuTest cpu;

	const u32 dividend = random<u32>();
	const u16 divisor = random<u16>();

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

TEST_CASE("idiv") {
	CpuTest cpu;

	const i32 dividend = random<i32>();
	const i16 divisor = random<i16>();

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

	REQUIRE_EQ(static_cast<i16>(cpu.m_regAR), ar_expected);
	REQUIRE_EQ(static_cast<i16>(cpu.m_regACL), acl_expected);
}