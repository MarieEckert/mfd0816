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

#ifndef MFDEMU_PLUGIN_API_HPP
#define MFDEMU_PLUGIN_API_HPP

#include <optional>

#include <mfdemu/impl/bus/aio_device.hpp>
#include <mfdemu/impl/bus/gio_device.hpp>

namespace mfdemu::plugin {

/* Dummy declarations because the config class doesn't exist yet */
class Config {};
class PluginDevice {};
class PluginAioDevice : public PluginDevice, public impl::AioDevice {};
class PluginGioDevice : public PluginDevice, public impl::GioDevice {};

class PluginAPI {
   public:
	const Config &getConfig() const;

	std::optional<std::shared_ptr<PluginDevice>> loadPluginDevice(const std::string &path);

   private:
	Config m_cfg;
	std::shared_ptr<PluginAioDevice> m_attachedAddressDevice;
	std::shared_ptr<PluginGioDevice> m_attachedGioDevice;
};

}  // namespace mfdemu::plugin

#endif