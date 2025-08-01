#include <memory>

#include <mfdemu/impl/bus/gio_device.hpp>
#include <mfdemu/impl/cpu.hpp>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include "doctest/doctest.h"

using namespace mfdemu;
using namespace mfdemu::impl;

class CpuTest : public Cpu {
   public:
	using CpuState = Cpu::CpuState;

	void newState(CpuState state) { Cpu::newState(state); }

	u16 &m_ioBusInput = Cpu::m_ioBusInput;
	u16 &m_ioBusOutput = Cpu::m_ioBusOutput;
	u16 &m_ioBusAddress = Cpu::m_ioBusAddress;
};

class GioDeviceTest : public GioDevice {
   public:
	GioDeviceTest() { m_data.resize(0xFFFF); }

	std::vector<u8> &data() { return m_data; }

   protected:
	void write(u16 address, u8 value, bool low) override {
		address = low ? address + 1 : address;
		m_data[address] = value;
	}

	u8 read(u16 address, bool low) override {
		address = low ? address + 1 : address;
		return m_data[address];
	}

	std::vector<u8> m_data;
};

TEST_CASE("gio write") {
	auto test_dev = std::make_shared<GioDeviceTest>();
	REQUIRE(test_dev != nullptr);

	CpuTest cpu;
	cpu.connectIoDevice(test_dev);
	cpu.newState(CpuTest::CpuState::GIO_WRITE);
	cpu.m_ioBusAddress = 0x7770;
	cpu.m_ioBusOutput = 0xfeed;

	cpu.iclck(); /* T0 */
	cpu.iclck(); /* T1 */
	cpu.iclck(); /* T2 */
	cpu.iclck(); /* T3 */
	cpu.iclck(); /* T4 */

	CHECK_EQ(test_dev->data().at(0x7770), 0xfe);
	CHECK_EQ(test_dev->data().at(0x7771), 0xed);
}

TEST_CASE("gio read") {
	auto test_dev = std::make_shared<GioDeviceTest>();
	REQUIRE(test_dev != nullptr);

	test_dev->data()[0x7770] = 0xfe;
	test_dev->data()[0x7771] = 0xed;

	CpuTest cpu;
	cpu.connectIoDevice(test_dev);
	cpu.newState(CpuTest::CpuState::GIO_READ);
	cpu.m_ioBusAddress = 0x7770;

	cpu.iclck(); /* T0 */
	cpu.iclck(); /* T1 */
	cpu.iclck(); /* T2 */
	cpu.iclck(); /* T3 */
	cpu.iclck(); /* T4 */

	REQUIRE_EQ(cpu.m_ioBusInput, 0xfeed);
}