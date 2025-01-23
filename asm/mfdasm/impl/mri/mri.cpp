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

#include <iostream>
#include <fstream>

#include <mfdasm/impl/mri/mri.hpp>
#include <mfdasm/log.hpp>

namespace mfdasm::impl::mri {

static std::vector<std::shared_ptr<Section>> makeSortedSectionList(const SectionTable &sections) {
	std::vector<std::shared_ptr<Section>> list;
	list.reserve(sections.sectionMap().size());

	for(const auto &[name, value] : sections.sectionMap()) {
		list.push_back(value);
	}

	std::sort(list.begin(), list.end(), [](const std::shared_ptr<Section> &a, const std::shared_ptr<Section> &b) {
		return a->offset < b->offset;
	});

	return list;
}

void writeCompactMRI(std::string path, SectionTable sections, bool compressed) {
}

void writePaddedMRI(std::string path, SectionTable sections, bool compressed) {
	std::vector<std::shared_ptr<Section>> section_list = makeSortedSectionList(sections);

	std::ofstream outfile;
	outfile.open(path);

	Header header = {
		.magic = {MRI_MAGIC},
		.version = MRI_VERSION,
		.type = 0,
		.filesize = BIGENDIAN32(static_cast<u32>(section_list.back()->offset + section_list.back()->data.size() + sizeof(Header))),
		.data_offset = BIGENDIAN32(sizeof(Header))
	};

	outfile.write((char*)&header, sizeof(header));

	logDebug() << "filesize: " << header.filesize << "\n";
	for(usize ix = 0; ix < section_list.size(); ix += 2) {
	//	if(ix + 1 >
	}

	outfile.close();
}

}
