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
/**
 * Class responsible for parsing and manipulating Minecraft server.properties files.
 */
class ServerPropertyFileParser {
	public:
		/**
		 * Struct used to describe a property within the server.properties file.
		 */
		struct Property {
			public:
				/**
				 * Holds the value of the property.
				 */
				std::string property;
				/**
				 * Holds the line number of the line the property is on.
				 */
				size_t line;
				/**
				 * Ensures that the line variable is initialized.
				 */
				Property() {
					line = 0;
				}
				/**
				 * Constructor.
				 * @param property
				 * @param line
				 */
				Property(std::string property, size_t line) :
						property(property), line(line) {
				}
				/**
				 * Equality operator.
				 * @param lhs
				 * @param rhs
				 * @return
				 */
				inline friend bool operator==(const Property& lhs, const Property& rhs) {
					return (lhs.property == rhs.property);
				}
				/**
				 * Inequality operator.
				 * @param lhs
				 * @param rhs
				 * @return
				 */
				inline friend bool operator!=(const Property& lhs, const Property& rhs) {
					return !(lhs == rhs);
				}
				/**
				 * Equality operator.
				 * @param lhs
				 * @param rhs
				 * @return
				 */
				inline friend bool operator==(const std::string& lhs, const Property& rhs) {
					return (lhs == rhs.property);
				}
				/**
				 * Inequality operator.
				 * @param lhs
				 * @param rhs
				 * @return
				 */
				inline friend bool operator!=(const std::string& lhs, const Property& rhs) {
					return !(lhs == rhs);
				}
				/**
				 * Equality operator.
				 * @param lhs
				 * @param rhs
				 * @return
				 */
				inline friend bool operator==(const Property& lhs, const std::string& rhs) {
					return (lhs.property == rhs);
				}
				/**
				 * Inequality operator.
				 * @param lhs
				 * @param rhs
				 * @return
				 */
				inline friend bool operator!=(const Property& lhs, const std::string& rhs) {
					return !(lhs == rhs);
				}
		};
	public:
		/**
		 * Copy constructor.
		 * @param parser
		 */
		ServerPropertyFileParser(const MinecraftServerDaemon::ServerPropertyFileParser& parser) :
				filename(parser.filename) {
			readProperties();
		}
		/**
		 * Opens the server.properties file and fills ServerPropertyFileParser::properties with data.
		 * @param file
		 */
		ServerPropertyFileParser(std::string file) :
				file(file.c_str(), std::ios::in | std::ios::out), filename(file) {
			readProperties();
		}
		/**
		 * Opens the server.properties file and fills ServerPropertyFileParser::properties with data.
		 * @param file
		 */
		ServerPropertyFileParser(char* file) :
				file(file, std::ios::in | std::ios::out), filename(file) {
			readProperties();
		}
		/**
		 * Flushes all changes to file and closes the file.
		 */
		~ServerPropertyFileParser() {
			flush();
			file.close();
		}
		/**
		 * Reads the properties from the property file.
		 */
		void readProperties() {
//			for (last_line = 1; !file.eof(); last_line++) {
//				std::string buffer;
//				size_t position;
//				std::string key;
//				std::getline(file, buffer);
//				position = buffer.find(=);
//				if (buffer.npos != position) {
//					Property value(buffer.substr(position + 1), last_line);
//					key = buffer.substr(0, position);
//					properties.insert(std::pair<std::string, Property>(key, value));
//				}
//			}
		}
		/**
		 * Modifies a property.
		 * @param key
		 * @param value
		 * @return
		 */
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
		/**
		 * Returns the value of a specified property.
		 * @param property
		 * @return
		 */
		std::string readProperty(std::string property) {
			readProperties();
			if (testForProperty(property)) {
				return properties[property].property;
			} else {
				return NULL;
			}
		}
		/**
		 * Adds a property to the property file. Returns true on success.
		 * @param key
		 * @param value
		 * @return
		 */
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
		/**
		 * Removes a property from the property file.  Returns true on success.
		 * @param key
		 * @return
		 */
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
		/**
		 * Tests for the existence of a specified property.
		 * @param property
		 * @return
		 */
		bool testForProperty(std::string property) {
			if (properties.find(property) == properties.end()) {
				return false;
			} else {
				return true;
			}
		}
		/**
		 * Flushes the properties stored in the internal map to file.
		 */
		void flush() {
			file.close();
			file.open(filename.c_str(), std::ios::in | std::ios::out | std::ios::trunc);
			for (std::map<std::string, Property>::iterator it = properties.begin(); it != properties.end(); ++it) {
				file << it->first << "=" << it->second.property << '\n';
			}
		}
	protected:
		/**
		 * Holds the line number of the last line of the property file.
		 */
		size_t last_line;
		/**
		 * File object for the property file.
		 */
		std::fstream file;
		/**
		 * Name of the property file.
		 */
		std::string filename;
		/**
		 * std::map storing the properties read from the property file.
		 */
		std::map<std::string, Property> properties;
};

} /* namespace MinecraftServerDaemon */
#endif /* DAEMON_SERVERPROPERTYFILEPARSER_HPP_ */
