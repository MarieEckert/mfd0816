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
#include <shared/log.hpp>
#include <shared/panic.hpp>

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
	if(m_addressDevice == nullptr) {
		shared::panic("m_addressDevice == nullptr");
	}

	switch(m_stateStep) {
	case 0: /* T0: priming pulse, AMS high */
		m_addressDevice->mode = true;
		m_addressDevice->clck();
		m_stateStep = 1;
		break;
	case 1: /* T1: address out, AMS high */
		m_addressDevice->mode = true;
		m_addressDevice->io = m_addressBusAddress;
		m_addressDevice->clck();
		m_stateStep = 2;
		break;
	case 2: /* T2: mode pulse, AMS low */
		m_addressDevice->mode = false;
		m_addressDevice->clck();
		m_stateStep = 3;
		break;
	case 3: /* T3: read pulse, AMS Low */
		m_addressDevice->mode = false;
		m_addressDevice->clck();
		m_addressBusInput = m_addressDevice->io;

		finishState();
		break;
	}
}

void Cpu::abusWrite() {
	if(m_addressDevice == nullptr) {
		shared::panic("m_addressDevice == nullptr");
	}

	switch(m_stateStep) {
	case 0: /* T0: priming pulse, AMS high */
		m_addressDevice->mode = true;
		m_addressDevice->clck();
		m_stateStep = 1;
		break;
	case 1: /* T1: address out, AMS high */
		m_addressDevice->mode = true;
		m_addressDevice->io = m_addressBusAddress;
		m_addressDevice->clck();
		m_stateStep = 2;
		break;
	case 2: /* T2: mode pulse, AMS high */
		m_addressDevice->mode = true;
		m_addressDevice->clck();
		m_stateStep = 3;
		break;
	case 3: /* T3: write pulse, AMS Low */
		m_addressDevice->mode = false;
		m_addressDevice->io = m_addressBusOutput;
		m_addressDevice->clck();

		finishState();
		break;
	}
}

void Cpu::gioRead() {
	if(m_ioDevice == nullptr) {
		shared::panic("m_ioDevice == nullptr");
	}

	switch(m_stateStep) {
	case 0: /* T0: Priming pulse, GMS high */
		m_ioDevice->mode = true;
		m_ioDevice->clck();

		m_stateStep = 1;
		break;
	case 1: /* T1: Address High-Byte pulse, GMS high */
		m_ioDevice->mode = true;
		m_ioDevice->io = (m_ioBusAddress >> 8) & 0xFF;
		m_ioDevice->clck();

		m_stateStep = 2;
		break;
	case 2: /* T2: Address Low-Byte pulse, GMS low */
		m_ioDevice->mode = false;
		m_ioDevice->io = m_ioBusAddress & 0xFF;
		m_ioDevice->clck();

		m_stateStep = 3;
		break;
	case 3: /* T3: Data High-Byte read pulse */
		m_ioDevice->clck();
		m_ioBusInput = m_ioDevice->io & 0xFF00;

		m_stateStep = 4;
		break;
	case 4: /* T4: Data Low-Byte read pulse */
		m_ioDevice->clck();
		m_ioBusInput |= m_ioDevice->io & 0xFF;

		finishState();
		break;
	}
}

void Cpu::gioWrite() {
	if(m_ioDevice == nullptr) {
		shared::panic("m_ioDevice == nullptr");
	}

	switch(m_stateStep) {
	case 0: /* T0: Priming pulse, GMS high */
		m_ioDevice->mode = true;
		m_ioDevice->clck();

		m_stateStep = 1;
		break;
	case 1: /* T1: Address High-Byte pulse, GMS high */
		m_ioDevice->mode = true;
		m_ioDevice->io = (m_ioBusAddress >> 8) & 0xFF;
		m_ioDevice->clck();

		m_stateStep = 2;
		break;
	case 2: /* T2: Address Low-Byte pulse, GMS high */
		m_ioDevice->mode = true;
		m_ioDevice->io = m_ioBusAddress & 0xFF;
		m_ioDevice->clck();

		m_stateStep = 3;
		break;
	case 3: /* T3: Data High-Byte write pulse */
		m_ioDevice->io = (m_ioBusOutput >> 8) & 0xFF;
		m_ioDevice->clck();

		m_stateStep = 4;
		break;
	case 4: /* T4: Data Low-Byte write pulse */
		m_ioDevice->io = m_ioBusOutput & 0xFF;
		m_ioDevice->clck();

		finishState();
		break;
	}
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

		m_regFL.rt = true;

		m_addressBusAddress = RESET_VECTOR;
		newState(CpuState::ABUS_READ);
		break;
	case 1:
		m_regIP = m_addressBusInput;

		m_regFL = {
			.of = false,
			.cf = false,
			.zf = false,
			.nf = false,
			.ie = false,
			.rt = false,
			.rs = false,
		};

		logDebug() << "\nreset, IP = " << std::hex << m_regIP << std::dec << "\n";

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
	if(m_stepStash.empty()) {
		m_stateStep = 0;

		while(!m_state.empty()) {
			m_state.pop();
		}

		m_state.push(CpuState::INST_FETCH);
		return;
	}

	m_stateStep = m_stepStash.top();
	m_stepStash.pop();
	m_state.pop();
}

}  // namespace mfdemu::impl