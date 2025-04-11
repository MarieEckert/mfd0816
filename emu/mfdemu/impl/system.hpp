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

#ifndef MFDEMU_IMPL_SYSTEM_HPP
#define MFDEMU_IMPL_SYSTEM_HPP

#include <memory>
#include <vector>

#include <shared/typedefs.hpp>

#include <mfdemu/impl/aio_device.hpp>
#include <mfdemu/impl/cpu.hpp>

namespace mfdemu::impl {
class System {
   public:
	System(u32 cycle_span, u16 main_memory_size);

	void setMainMemoryData(std::vector<u8> data);

	void run();

   private:
	u32 m_cycleSpan;
	Cpu m_cpu;
	std::shared_ptr<AioDevice> m_mainMemory;
	/* AsciiConsole m_console; */
};
}  // namespace mfdemu::impl

#endif