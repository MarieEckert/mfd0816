#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest/doctest.h>

#include "test_cpu.hpp"

#include <mfdemu/impl/bus/aio_device.hpp>

using namespace ::mfdemu::impl;

namespace test::mfdemu {
TEST_SUITE("Interrupt") {
	TEST_CASE("hardware interrupt") {
		std::vector<u8> main_memory(64 * 1024);
		main_memory[0x1234] = 0x10;
		main_memory[0x1235] = 0x00;
		main_memory[0x1236] = 0x00;
		main_memory[0x1237] = 0x00;
		main_memory[0xFFFC] = 0x12;
		main_memory[0xFFFD] = 0x34;
		main_memory[0xFFFE] = 0x00;
		main_memory[0xFFFF] = 0x00;
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

		/* buffer cycle */
		cpu.iclck();

		/* IRA */
		cpu.iclck();  // IRA T1
		REQUIRE(cpu.ira());
		cpu.iclck();  // IRA T2
		REQUIRE(cpu.ira());
		cpu.iclck();  // IRA done, internal interrupt processing

		// run until the cpu is about to enter the interrupt vector
		while(cpu.m_regFL.ie) {
			cpu.iclck();
		}

		REQUIRE_EQ(cpu.m_regIP, 0x1234);
	}
	TEST_CASE("software interrupt") {}
}
}  // namespace test::mfdemu
