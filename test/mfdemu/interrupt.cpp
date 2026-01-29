#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest/doctest.h>

#include "test_cpu.hpp"

#include <mfdemu/impl/bus/aio_device.hpp>

using namespace ::mfdemu::impl;

namespace test::mfdemu {
static void test(CpuTest &cpu, u16 iid, u16 iv, u16 ret) {
	// run until the cpu is about to enter the interrupt vector
	while(cpu.m_regFL.ie) {
		cpu.iclck();
	}

	CHECK_EQ(cpu.m_regIID, iid);
	REQUIRE_EQ(cpu.m_regIP, iv);

	// Test IRET
	for(u8 i = 0; i < 16; i++) {
		cpu.iclck();
	}

	CHECK_EQ(cpu.m_regIID, 0x0000);
	REQUIRE_EQ(cpu.m_regIP, ret);
}

static void setMemory(std::vector<u8> &mem) {
	mem[0x0000] = 0x10;
	mem[0x0001] = 0x00;
	mem[0x0002] = 0x00;
	mem[0x0003] = 0x00;
	mem[0x1234] = 0x0f;
	mem[0x1235] = 0x00;
	mem[0xFFFC] = 0x12;
	mem[0xFFFD] = 0x34;
	mem[0xFFFE] = 0x00;
	mem[0xFFFF] = 0x00;
}

TEST_SUITE("Interrupt") {
	TEST_CASE("hardware interrupt") {
		std::vector<u8> main_memory(64UL * 1024);
		setMemory(main_memory);
		auto main_memory_dev = std::make_shared<AioDevice>(false, 64 * 1024);
		main_memory_dev->setData(std::move(main_memory));
		REQUIRE(main_memory_dev != nullptr);

		CpuTest cpu;
		cpu.connectAddressDevice(main_memory_dev);
		cpu.reset = true;
		cpu.iclck();
		cpu.reset = false;

		/* run the cpu for some time */
		for(u8 i = 0; i < 16; i++) {
			cpu.iclck();
		}

		cpu.m_regFL.ie = true;

		/* IRQ */
		cpu.irq = true;
		cpu.iclck();  // initial cpu reaction time
		cpu.irq = false;

		/* wait for CPU to switch state & IRA T1.
		 * Might take a moment since its finishing off the current instruction.
		 * This will also include T1 of the IRA.
		 */
		while(cpu.m_regFL.iq) {
			cpu.iclck();
		}

		REQUIRE(cpu.ira());	 // IRA T1
		cpu.iclck();		 // IRA T2
		REQUIRE(cpu.ira());
		cpu.iclck();  // IRA done, internal interrupt processing

		// TODO: Provide an IO device to properly test IRA sequence
		const u16 fake_iid = 0x0012;
		cpu.m_regIID = fake_iid;
		test(cpu, fake_iid, 0x1234, 0x004);
	}
	TEST_CASE("software interrupt") {
		std::vector<u8> main_memory(64UL * 1024);
		setMemory(main_memory);
		main_memory[0x0000] = 0x0e;
		main_memory[0x0003] = 0x12;
		auto main_memory_dev = std::make_shared<AioDevice>(false, 64 * 1024);
		main_memory_dev->setData(std::move(main_memory));
		REQUIRE(main_memory_dev != nullptr);

		CpuTest cpu;
		cpu.connectAddressDevice(main_memory_dev);

		cpu.reset = true;
		cpu.iclck();
		cpu.reset = false;

		/* last thing reset does is clear all flags. */
		cpu.m_regFL.ie = true;
		while(cpu.m_regFL.ie) {
			cpu.iclck();
		}

		cpu.m_regFL.ie = true;

		/* run until interrupts disabled. means we are processing one. */
		while(cpu.m_regFL.ie) {
			cpu.iclck();
		}

		for(u8 i = 0; i < 3; i++) {
			cpu.iclck();
		}

		// TODO: Provide an IO device to properly test IRA sequence
		const u16 fake_iid = 0x0012;
		test(cpu, fake_iid, 0x1234, 0x004);
	}
}
}  // namespace test::mfdemu
