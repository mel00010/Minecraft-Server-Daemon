/*******************************************************************************
 *
 * Minecraft Server Daemon
 * ServerPropertyFileParser.hpp
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

#ifndef DAEMON_SERVERPROPERTYFILEPARSER_HPP_
#define DAEMON_SERVERPROPERTYFILEPARSER_HPP_

#include <stddef.h>
#include <fstream>
#include <map>
#include <string>
#include <utility>

namespace MinecraftServerDaemon {

class ServerPropertyFileParser {
	public:
		/* namespace MinecraftServerDaemon */
		struct Property {
				std::string property;
				size_t line;
				Property() {
					line = 0;
				}
				Property(std::string property, size_t line) :
						property(property), line(line) {
				}
				inline friend bool operator==(const Property& lhs, const Property& rhs) {
					return (lhs.property == rhs.property);
				}
				inline friend bool operator!=(const Property& lhs, const Property& rhs) {
					return !(lhs == rhs);
				}
				inline friend bool operator==(const std::string& lhs, const Property& rhs) {
					return (lhs == rhs.property);
				}
				inline friend bool operator!=(const std::string& lhs, const Property& rhs) {
					return !(lhs == rhs);
				}
				inline friend bool operator==(const Property& lhs, const std::string& rhs) {
					return (lhs.property == rhs);
				}
				inline friend bool operator!=(const Property& lhs, const std::string& rhs) {
					return !(lhs == rhs);
				}
		};
	public:
		ServerPropertyFileParser(const MinecraftServerDaemon::ServerPropertyFileParser& parser) :
				filename(parser.filename) {
			readProperties();
		}
		ServerPropertyFileParser(std::string file) :
				file(file.c_str(), std::ios::in | std::ios::out), filename(file) {
			readProperties();
		}
		ServerPropertyFileParser(char* file) :
				file(file, std::ios::in | std::ios::out), filename(file) {
			readProperties();
		}
		~ServerPropertyFileParser() {
			flush();
			file.close();
		}
		void readProperties() {
//			for (last_line = 1; !file.eof(); last_line++) {
//				std::string buffer;
//				size_t position;
//				std::string key;
//				std::getline(file, buffer);
//				position = buffer.find("=");
//				if (buffer.npos != position) {
//					Property value(buffer.substr(position + 1), last_line);
//					key = buffer.substr(0, position);
//					properties.insert(std::pair<std::string, Property>(key, value));
//				}
//			}
		}
		bool modifyProperty(std::string key, std::string value) {
			readProperties();
			if (testForProperty(key)) {
				std::map<std::string, Property>::mapped_type buffer = properties[key];
				buffer.property = value;
				properties.erase(key);
				properties.insert(std::pair<std::string, Property>(key, buffer));
				flush();
				return true;
			} else {
				return false;
			}
		}
		std::string readProperty(std::string property) {
			readProperties();
			if (testForProperty(property)) {
				return properties[property].property;
			} else {
				return NULL;
			}
		}
		bool addProperty(std::string key, std::string value) {
			readProperties();
			if (testForProperty(key)) {
				return false;
			} else {
				Property buffer;
				buffer.property = value;
				buffer.line = last_line + 1;
				properties.insert(std::pair<std::string, Property>(key, buffer));
				flush();
				return true;
			}
		}
		bool removeProperty(std::string key) {
			readProperties();
			if (testForProperty(key)) {
				properties.erase(key);
				flush();
				return true;
			} else {
				return false;
			}
		}
		bool testForProperty(std::string property) {
			if (properties.find(property) == properties.end()) {
				return false;
			} else {
				return true;
			}
		}
		void flush() {
			file.close();
			file.open(filename.c_str(), std::ios::in | std::ios::out | std::ios::trunc);
			for (std::map<std::string, Property>::iterator it = properties.begin(); it != properties.end(); ++it) {
				file << it->first << "=" << it->second.property << '\n';
			}
		}
	protected:
		size_t last_line;
		std::fstream file;
		std::string filename;
		std::map<std::string, Property> properties;
};

} /* namespace MinecraftServerDaemon */
#endif /* DAEMON_SERVERPROPERTYFILEPARSER_HPP_ */
