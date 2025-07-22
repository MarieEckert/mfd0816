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

#include <mfdemu/impl/bus/gio_device.hpp>

namespace mfdemu::impl {

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
		m_write = mode;
		m_step = 3;
		break;
	case 3: /* T3 */
		if(m_write) {
			write(m_address, io, false);
		} else {
			io = read(m_address, false);
		}

		m_step = 4;
		break;
	case 4: /* T4 */
		if(m_write) {
			write(m_address, io, true);
		} else {
			io = read(m_address, true);
		}

		m_step = 0;
		break;
	}
}

}  // namespace mfdemu::impl
