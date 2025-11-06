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

/**
 * @file mfdemu_api.h
 * @brief Type and Function-Prototype definitions for the MFDEMU device plugin
 * API.
 */

#ifndef __MFDEMU_API_H
#define __MFDEMU_API_H

#ifdef __cplusplus
namespace mfdemu::api {
#endif

#include <stdint.h>

#define MFDEMU_DEVICE_API_H_VERSION 0

extern uint32_t mfdemuApiVersion;

/**
 * @brief Used to represent the current state of CPU pins relevant for Address
 * Bus devices.
 */
typedef struct {
	/**
	 * @brief The current state of the AIO pins. This value may be modified, but
	 * modifications are ignored unless the CPU is currently at the T3 stage of
	 * an Address Bus read.
	 */
	u16 busPins;

	/**
	 * @brief The current value of the AMS pin.
	 */
	u8 mode;
	u8 pad;
} AioState;

/**
 * @brief Used to represent the current state of CPU pins relevant for I/O Bus
 * devices.
 */
typedef struct {
	/**
	 * @brief The current state of the GIO pins. This value may be modified, but
	 * modifications are ignored unless the CPU is currently at the T3 or T4
	 * stage of an I/O Bus read.
	 */
	u8 busPins;

	/**
	 * @brief The current value of the GMS pin.
	 */
	u8 mode;
	u16 pad;
} GioState;

/**
 * @brief The possible datatypes contained within a MFDEMU configuration.
 */
typedef enum {
	String,

	/**
	 * @brief Corresponds to the C++ default bool type.
	 */
	Boolean,

	/**
	 * @brief Corresponds to the systems default character type.
	 */
	Character,

	/**
	 * @brief Corresponds to ínt64_t.
	 */
	Integer,

	/**
	 * @brief Corresponds to uint64_t.
	 */
	Unsigned,

	/**
	 * @brief Corresponds to 32-bit floats.
	 */
	Float,

	/**
	 * @brief Corresponds to the native pointer type. If the pointer is valid,
	 * the underlying value's type corresponds to the type used for JSON objects
	 * by the used JSON implementation.
	 */
	Object,
} ConfigDataType;

/**
 * @brief A result from a config inquiry.
 */
typedef struct {
	ConfigDataType type;

	/**
	 * @brief If value is nullptr, the result is invalid.
	 */
	void *value;
} ConfigValueResult;

/**
 * @defgroup EmuApi-EmuCalls Functions called by the Emulator.
 * @{
 */

/**
 * @brief Used to signal that the CLK pin connected to the address bus device has been
 * pulled high.
 */
void AioClk(AioState *state);

/**
 * @brief Used to signal that the CLK pin connected to the i/o bus device has been
 * pulled high.
 */
void GioClk(GioState *state);

/**
 * @brief Used to initialise a device plugin.
 * @param handle The handle to the API.
 */
void DeviceInit(APIHandle handle);

/** @} */
/**
 * @defgroup EmuApi-DevCalls Functions called by Devices.
 * @{
 */

/**
 * @brief Used to get a value from the emulators configuration.
 * @param handle The API Handle.
 * @param nameLength The name of the proceeding name string.
 * @param name The name of the value to get.
 * @param baseObject Optionally set the object from which the value should be
 * acquired. Only pointers returned from this function with the associated
 * `ConfigDataType` of `Object` can possibly be valid.
 */
ConfigValueResult
GetConfigValue(APIHandle handle, size_t nameLength, const char *name, void *baseObject = nullptr);

/** @} */

#ifdef __cplusplus
}
#endif
#endif
