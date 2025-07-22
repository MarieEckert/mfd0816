/*
 * Copyright (C) 2025  Marie Eckert
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
#include <shared/panic.hpp>

#include <mfdemu/impl/bus/gio_device.hpp>

namespace mfdemu::impl {

GioDevice::GioDevice(bool read_only, usize size) : m_readOnly(read_only) {
	m_data.resize(size);
}

void GioDevice::clck() {
	switch(m_step) {
	case 0: /* T0 */
		if(mode) {
			m_step = 1;
		}
		break;
	case 1: /* T1 */
		if(!mode) {
			m_step = 0;
			break;
		}

		m_address = io << 8;
		m_step = 2;
		break;
	case 2: /* T2 */
		m_address |= io;

		logDebug() << "write to address now: " << ((u64)m_address) << "\n";
		m_write = mode;
		m_step = 3;
		break;
	case 3: /* T3 */
		if(m_write) {
			if(m_readOnly) {
				m_step = 0;
				break;
			}

			if(m_address >= m_data.size() || m_address + 1 >= m_data.size()) { /* discard */
				m_step = 0;
				break;
			}

			logDebug() << "write first halve now: " << ((u64)io) << "\n";
			m_data[m_address] = io;
		} else {
			io = m_address >= m_data.size() || m_address + 1 <= m_data.size() ? 0
																			  : m_data[m_address];
		}

		m_step = 4;
		break;
	case 4: /* T4 */
		if(m_write) {
			logDebug() << "write second halve now: " << ((u64)io) << "\n";
			m_data[m_address + 1] = io;
		} else {
			io = m_data[m_address + 1];
			break;
		}

		logDebug() << "finished!!!\n";
		shared::panic("test");
		m_step = 0;
		break;
	}
}

void GioDevice::setData(std::vector<u8> data) {
	m_data = data;
}

}  // namespace mfdemu::impl
