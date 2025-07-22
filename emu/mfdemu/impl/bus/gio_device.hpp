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

#ifndef MFDEMU_IMPL_GIO_DEVICE_HPP
#define MFDEMU_IMPL_GIO_DEVICE_HPP

#include <shared/typedefs.hpp>

#include <mfdemu/impl/bus/bus_device.hpp>

namespace mfdemu::impl {

class GioDevice : public BaseBusDevice<u8> {
   public:
	GioDevice() = default;
	void clck() override;

   protected:
	virtual void write(u16 address, u8 value, bool low) = 0;
	virtual u8 read(u16 address, bool low) = 0;

	/** internal state */
	u8 m_step{0};
	u16 m_address{0};
	bool m_write{false};
};

}  // namespace mfdemu::impl

#endif
