#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include <doctest/doctest.h>

#include "test_cpu.hpp"

#include <mfdemu/impl/bus/aio_device.hpp>

using namespace ::mfdemu::impl;

namespace test::mfdemu {
TEST_SUITE("Interrupt") {
	TEST_CASE("hardware interrupt") {
		std::vector<u8> main_memory(64 * 1024);
		main_memory[0x0000] = 0x10;
		main_memory[0x0001] = 0x00;
		main_memory[0x0002] = 0x00;
		main_memory[0x0003] = 0x00;
		main_memory[0xFFFC] = 0x00;
		main_memory[0xFFFD] = 0x00;
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

		cpu.irq = true;
		cpu.iclck(); /* initial cpu reaction time */
		cpu.iclck(); /* buffer cycle */
		cpu.iclck(); /* start of IRA (IRA T1) */
		REQUIRE(cpu.ira());
		cpu.iclck(); /* IRA T2 */
		REQUIRE(cpu.ira());
		cpu.iclck(); /* IRA done, internal interrupt processing */
	}
	TEST_CASE("software interrupt") {}
}
}  // namespace test::mfdemu
