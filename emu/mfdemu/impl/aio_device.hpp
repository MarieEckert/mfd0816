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

#ifndef MFDEMU_IMPL_AIO_DEVICE_HPP
#define MFDEMU_IMPL_AIO_DEVICE_HPP

#include <vector>

#include <mfdemu/impl/bus_device.hpp>
#include <shared/typedefs.hpp>

namespace mfdemu::impl {

class AioDevice : public BaseBusDevice {
   public:
	AioDevice(bool read_only, usize size);
	void clck() override;

	void setData(std::vector<u8> data);

   private:
	bool m_readOnly;
	std::vector<u8> m_data;
};

}  // namespace mfdemu::impl

#endif
