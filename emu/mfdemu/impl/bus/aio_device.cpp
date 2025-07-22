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

#include <shared/log.hpp>

#include <mfdemu/impl/bus/aio_device.hpp>

namespace mfdemu::impl {

AioDevice::AioDevice(bool read_only, usize size) : m_readOnly(read_only) {
	m_data.reserve(size);
}

void AioDevice::clck() {
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
			if(m_readOnly) {
				m_step = 0;
				break;
			}

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

void AioDevice::setData(std::vector<u8> data) {
	m_data = data;
}

}  // namespace mfdemu::impl
