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

#include <fstream>
#include <iostream>
#include <memory>

#include <mfdasm/impl/mri/mri.hpp>
#include <shared/int_ops.hpp>
#include <shared/log.hpp>

#include "mfdasm/impl/mri/section_table.hpp"

namespace mfdasm::impl::mri {

static std::vector<std::shared_ptr<Section>> makeSortedSectionList(const SectionTable &sections) {
	std::vector<std::shared_ptr<Section>> list;
	list.reserve(sections.sectionMap().size());

	for(const auto &[name, value]: sections.sectionMap()) {
		list.push_back(value);
	}

	std::sort(
		list.begin(), list.end(),
		[](const std::shared_ptr<Section> &a, const std::shared_ptr<Section> &b) {
			return a->offset < b->offset;
		});

	return list;
}

void writeCompactMRI(std::string path, SectionTable sections, bool compressed) {
	std::vector<std::shared_ptr<Section>> section_list = makeSortedSectionList(sections);

	std::ofstream outfile;
	outfile.open(path);

	Header header = {
		.magic = {MRI_MAGIC},
		.version = MRI_VERSION,
		.type = static_cast<u16>(TypeFlag::COMPACT),
		.filesize = 0,
		.data_offset = BIGENDIAN32(sizeof(Header))};

	const u32 base_filesize =
		sizeof(header) + sizeof(u32) + (sizeof(TableEntry) * section_list.size());

	std::vector<TableEntry> tables_entries;
	u32 offset = base_filesize;
	for(const std::shared_ptr<Section> &section: section_list) {
		const u16 section_size = section->data.size();
		TableEntry entry = {
			.file_offset = BIGENDIAN32(offset),
			.load_address = BIGENDIAN16(section->offset),
			.length = BIGENDIAN16(section_size),
		};

		tables_entries.push_back(entry);

		offset += section_size;
	}

	header.filesize = BIGENDIAN32(offset);

	outfile.write(reinterpret_cast<char *>(&header), sizeof(header));

	const u32 tablesize_be = BIGENDIAN32(tables_entries.size());
	outfile.write(reinterpret_cast<const char *>(&tablesize_be), sizeof(tablesize_be));

	for(TableEntry &table_entry: tables_entries) {
		outfile.write(reinterpret_cast<char *>(&table_entry), sizeof(table_entry));
	}

	for(const std::shared_ptr<Section> &section: section_list) {
		outfile.write(reinterpret_cast<char *>(section->data.data()), section->data.size());
	}

	outfile.close();
}

void writePaddedMRI(std::string path, SectionTable sections, bool compressed) {
	std::vector<std::shared_ptr<Section>> section_list = makeSortedSectionList(sections);

	std::ofstream outfile;
	outfile.open(path);

	Header header = {
		.magic = {MRI_MAGIC},
		.version = MRI_VERSION,
		.type = 0,
		.filesize = BIGENDIAN32(static_cast<u32>(
			section_list.back()->offset + section_list.back()->data.size() + sizeof(Header))),
		.data_offset = BIGENDIAN32(sizeof(Header))};

	outfile.write(reinterpret_cast<char *>(&header), sizeof(header));

	logDebug() << "filesize: " << BIGENDIAN32(header.filesize) << "\n";
	for(usize ix = 0; ix < section_list.size(); ix++) {
		std::shared_ptr<Section> section = section_list[ix];

		if(ix == 0) {
			std::fill_n(std::ostream_iterator<char>(outfile), section->offset, 0);
		}

		logDebug() << "current section offset: " << static_cast<usize>(section->offset)
				   << ", size: " << static_cast<usize>(section->data.size()) << "\n";

		outfile.write(reinterpret_cast<char *>(section->data.data()), section->data.size());

		if(ix + 1 >= section_list.size()) {
			break;
		}

		logDebug() << "filling "
				   << (section_list[ix + 1]->offset - (section->data.size() + section->offset))
				   << " bytes\n";

		std::fill_n(
			std::ostream_iterator<char>(outfile),
			section_list[ix + 1]->offset - (section->data.size() + section->offset), 0);
	}

	outfile.close();
}

}  // namespace mfdasm::impl::mri
