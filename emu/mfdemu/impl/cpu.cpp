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

#include <memory>

#include <mfdemu/impl/bus_device.hpp>
#include <mfdemu/impl/cpu.hpp>

namespace mfdemu::impl {

/** class Cpu **/

void Cpu::connectAddressDevice(std::shared_ptr<BaseBusDevice> device) {
	m_addressDevice = device;
}

void Cpu::connectIoDevice(std::shared_ptr<BaseBusDevice> device) {
	m_ioDevice = device;
}

void Cpu::iclck() {
	if(reset) {
		while(!m_state.empty()) {
			m_state.pop();
		}

		m_state.push(CpuState::INST_FETCH);
		m_state.push(CpuState::RESET);
		m_stateStep = 0;
	}

	switch(m_state.top()) {
	case CpuState::ABUS_READ:
		this->abusRead();
		break;
	case CpuState::ABUS_WRITE:
		this->abusWrite();
		break;
	case CpuState::GIO_READ:
		this->gioRead();
		break;
	case CpuState::GIO_WRITE:
		this->gioWrite();
		break;
	case CpuState::INST_FETCH:
		this->fetchInst();
		break;
	case CpuState::INST_EXEC:
		this->execInst();
		break;
	case CpuState::RESET:
		this->execReset();
		break;
	}
}

void Cpu::abusRead() {
	/** @todo implement */
}

void Cpu::abusWrite() {
	/** @todo implement */
}

void Cpu::gioRead() {
	/** @todo implement */
}

void Cpu::gioWrite() {
	/** @todo implement */
}

void Cpu::fetchInst() {
	/** @todo implement */
}

void Cpu::execInst() {
	/** @todo implement */
}

void Cpu::execReset() {
	switch(m_stateStep) {
	case 0:
		m_stateStep = 1;

		newState(CpuState::ABUS_READ);
		break;
	case 1:
		m_regIP = m_addressBusInput;

		finishState();
		break;
	}
}

void Cpu::newState(CpuState state) {
	m_state.push(state);
	m_stepStash.push(m_stateStep);
	m_stateStep = 0;
}

void Cpu::finishState() {
	m_stateStep = m_stepStash.top();
	m_stepStash.pop();
	m_state.pop();
}

}  // namespace mfdemu::impl