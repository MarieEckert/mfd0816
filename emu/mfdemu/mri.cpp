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

#include <cstring>

#include <mfdemu/mri.hpp>
#include <shared/log.hpp>

#define CHECK_BIT(value, bit) ((value & static_cast<u64>(bit)) != 0)

using namespace shared::mri_types;

namespace mfdemu {

std::vector<u8> parseCompact(const std::vector<u8> &data, const Header &header) {
	const usize base_required_size = MRI_MIN_SIZE + sizeof(u32);
	if(data.size() < base_required_size) {
		logError() << "invalid MRI: input smaller than minimal needed size (2)\n";
		std::exit(1);
	}

	const u32 entry_count =
		(data[TABLE_ENTRY_COUNT_OFFSET] << 24) | (data[TABLE_ENTRY_COUNT_OFFSET + 1] << 16) |
		(data[TABLE_ENTRY_COUNT_OFFSET + 2] << 8) | data[TABLE_ENTRY_COUNT_OFFSET + 3];
	const usize required_size = base_required_size + entry_count * sizeof(TableEntry);

	logDebug() << "required size: " << required_size << "\n";

	if(data.size() < required_size) {
		logError() << "invalid MRI: input smaller than minimal needed size (3)\n";
		std::exit(1);
	}

	std::vector<TableEntry> entries;
	entries.reserve(entry_count);

	for(u32 ix = 0; ix < entry_count; ix++) {
		TableEntry entry;
		std::memcpy(
			&entry, data.data() + TABLE_ENTRY_START_OFFSET + ix * sizeof(entry), sizeof(entry));

		entry.file_offset = BIGENDIAN32(entry.file_offset);
		entry.load_address = BIGENDIAN16(entry.load_address);
		entry.length = BIGENDIAN16(entry.length);
		entries.push_back(entry);
	}

	std::vector<u8> result;
	result.reserve(UINT16_MAX + 1);
	result.insert(result.begin(), result.capacity(), 0);

	for(const TableEntry &entry: entries) {
		logDebug() << "file offset: " << static_cast<u32>(entry.file_offset)
				   << ", length: " << static_cast<u32>(entry.length)
				   << ", load address: " << static_cast<u32>(entry.load_address)
				   << ", size: " << data.size() << ", destsize: " << result.capacity() << "\n";

		if(entry.file_offset >= data.size() ||
		   entry.file_offset + entry.length - 1 >= data.size()) {
			logError() << "invalid MRI: unexpected end of input\n";
			std::exit(1);
		}

		for(u32 ix = 0; ix < entry.length; ix++) {
			result[static_cast<usize>(entry.load_address + ix)] = data[entry.file_offset + ix];
		}
	}

	return result;
}

std::vector<u8> parseMRIFromBytes(const std::vector<u8> &data) {
	if(data.size() < MRI_MIN_SIZE) {
		logError() << "invalid MRI: input smaller than minimal needed size (1)\n";
		std::exit(1);
	}

	std::vector<u8> result;

	Header header;
	std::memcpy(&header, data.data(), sizeof(header));

	if(std::strcmp(header.magic, MRI_MAGIC) != 0) {
		logError() << "invalid MRI: invalid magic\n";
		std::exit(1);
	}

	if(header.version != MRI_VERSION) {
		logError() << "invalid MRI: version mismatch, supported version: " << std::hex
				   << MRI_VERSION << std::dec << "\n";
		std::exit(1);
	}

	header.data_offset = BIGENDIAN32(header.data_offset);

	if(CHECK_BIT(header.type, TypeFlag::COMPACT)) {
		return parseCompact(data, header);
	}

	return std::vector<u8>(data.begin() + header.data_offset, data.end());
}

}  // namespace mfdemu