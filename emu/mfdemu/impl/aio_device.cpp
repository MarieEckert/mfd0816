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

#include <mfdemu/impl/aio_device.hpp>

namespace mfdemu::impl {

AioDevice::AioDevice(bool read_only, usize size) : m_readOnly(read_only) {
	m_data.reserve(size);
}

/** @todo implement */
void AioDevice::clck() {}

void AioDevice::setData(std::vector<u8> data) {
	m_data = data;
}

}  // namespace mfdemu::impl
