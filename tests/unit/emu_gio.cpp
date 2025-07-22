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

	u16 m_ioBusInput{0};
	u16 m_ioBusOutput{0};
	u16 m_ioBusAddress{0};
};

class GioDeviceTest : public GioDevice {
   public:
	GioDeviceTest(bool read_only, usize size) : GioDevice(read_only, size) {}

	std::vector<u8> &data() { return m_data; }
};

TEST_CASE("gio write") {
	auto test_dev = std::make_shared<GioDeviceTest>(false, 0xffff);
	REQUIRE(test_dev != nullptr);

	CpuTest cpu;
	cpu.connectIoDevice(test_dev);
	cpu.newState(CpuTest::CpuState::GIO_WRITE);
	cpu.m_ioBusAddress = 0x7770;
	cpu.m_ioBusOutput = 0xfeed;

	cpu.iclck();
	cpu.iclck();
	cpu.iclck();
	cpu.iclck();
	cpu.iclck();

	CHECK_EQ(test_dev->data().at(0x7770), 0xfe);
	CHECK_EQ(test_dev->data().at(0x7771), 0xed);
}

TEST_CASE("gio read") {
	auto test_dev = std::make_shared<GioDeviceTest>(false, 0xffff);
	REQUIRE(test_dev != nullptr);

	test_dev->data()[0x7770] = 0xfe;
	test_dev->data()[0x7771] = 0xed;

	CpuTest cpu;
	cpu.connectIoDevice(test_dev);
	cpu.newState(CpuTest::CpuState::GIO_READ);
	cpu.m_ioBusAddress = 0x7770;

	cpu.iclck();
	cpu.iclck();
	cpu.iclck();
	cpu.iclck();
	cpu.iclck();

	REQUIRE_EQ(cpu.m_ioBusInput, 0xfeed);
}