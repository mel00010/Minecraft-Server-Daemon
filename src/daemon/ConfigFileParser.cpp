/*******************************************************************************
 *
 * Minecraft Server Daemon
 * ConfigFileParser.cpp
 * Copyright (C) 2016  Mel McCalla <melmccalla@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 *
 *******************************************************************************/

#include <ConfigFileParser.hpp>
#include <cstdlib>
#include <fstream>
#include <new>
#include <json/writer.h>
#include <json/reader.h>

/**
 * Parses the daemon config file and returns a Json::Value object.
 * @param configFile
 * @return
 */
Json::Value ConfigFileParser::parseConfigFile(std::string configFile) {
	Json::Value root;
	Json::Reader reader;
	try {
		reader.parse(getFileContents(configFile.c_str()), root);
	} catch (std::bad_alloc& e) {
		exit(1);
	}
	return root;
}
/**
 * Gets the contents of the daemon config file.
 * @param filename
 * @return
 */
std::string ConfigFileParser::getFileContents(const char *filename) {
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (in) {
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return (contents);
	} else {
		exit(1);
	}
}

