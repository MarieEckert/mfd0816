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

#include <cassert>
#include <iostream>
#include <memory>
#include <ostream>
#include <vector>

#include <shared/log.hpp>

#include <mfdemu/impl/aio_device.hpp>
#include <mfdemu/impl/system.hpp>

namespace mfdemu::impl {

System::System(u32 cycle_span, u16 main_memory_size)
	: m_cycleSpan(cycle_span), m_mainMemory(std::make_shared<AioDevice>(false, main_memory_size)) {
	m_cpu.connectAddressDevice(m_mainMemory);
}

void System::setMainMemoryData(std::vector<u8> data) {
	m_mainMemory->setData(data);
}

void System::run() {
	struct timespec ts;
	u64 last_time = 0;

#ifdef SHOW_CYCLES
	u64 last_speed_time = 0;
	u64 cycles = 0;
#endif

	/* trigger reset */
	m_cpu.reset = true;
	m_cpu.iclck();
	m_cpu.reset = false;

	while(true) {
		assert(clock_gettime(CLOCK_MONOTONIC, &ts) == 0);
		const u64 current_time = (ts.tv_sec * (1000 * 1000 * 1000)) + ts.tv_nsec;

#ifdef SHOW_CYCLES
		if(current_time - last_speed_time > (1000 * 1000 * 1000)) {
			std::cout << "\r" << cycles << " Hz" << std::flush;
			last_speed_time = current_time;
			cycles = 0;
		}
#endif

		if(current_time - last_time < m_cycleSpan) {
			continue;
		}

#ifdef SHOW_CYCLES
		cycles++;
#endif

		last_time = current_time;

		m_cpu.iclck();
	}
}

}  // namespace mfdemu::impl
