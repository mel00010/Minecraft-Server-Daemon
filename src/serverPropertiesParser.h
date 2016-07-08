#ifndef SERVER_PROPERTIES_PARSER_H
#define SERVER_PROPERTIES_PARSER_H
#include <string>
#include <map>
#include <fstream>

namespace MinecraftServerService {
class ServerPropertiesParser {
	public:
		struct Property {
			std::string property;
			size_t line;
			Property(){};
			Property(std::string property, size_t line) : property(property), line(line) {};
			inline friend bool operator==(const Property& lhs, const Property& rhs){ return (lhs.property == rhs.property);};
			inline friend bool operator!=(const Property& lhs, const Property& rhs){ return !(lhs == rhs); };
			inline friend bool operator==(const std::string& lhs, const Property& rhs){ return (lhs == rhs.property);};
			inline friend bool operator!=(const std::string& lhs, const Property& rhs){ return !(lhs == rhs); };
			inline friend bool operator==(const Property& lhs, const std::string& rhs){ return (lhs.property == rhs);};
			inline friend bool operator!=(const Property& lhs, const std::string& rhs){ return !(lhs == rhs); };
		};
	public:
		ServerPropertiesParser(std::string file) : file(file, std::ios::in|std::ios::out), filename(file) {
			readProperties();
		}
		~ServerPropertiesParser() {
			flush();
			file.close();
		}
		void readProperties() {
			for (last_line = 1 ; !file.eof() ; last_line++)
			{
				std::string buffer;
				size_t position;
				std::string key;
				std::getline(file, buffer);
				buffer.find("=");
				if(buffer.npos != position) {
					Property value(buffer.substr(position + 1), last_line);
					key = buffer.substr(0, position);
					properties.insert(std::pair<std::string,Property>(key,value));
				}
			}
		};
		bool modifyProperty(std::string key, std::string value) {
			readProperties();
			if (testForProperty(key)) {
				std::map<std::string, Property>::mapped_type buffer = properties[key];
				buffer.property=value;
				properties.erase(key);
				properties.insert(std::pair<std::string,Property>(key,buffer));
				flush();
				return true;
			} else {
				return false;
			}
		};
		std::string readProperty(std::string property) {
			readProperties();
			if (testForProperty(property)) {
				return properties[property].property;
			} else {
				return NULL;
			}
		};
		bool addProperty(std::string key, std::string value) {
			readProperties();
			if (testForProperty(key)) {
				return false;
			} else {
				Property buffer = properties[key];
				buffer.property=value;
				buffer.line=last_line+1;
				properties.insert(std::pair<std::string,Property>(key,buffer));
				flush();
				return true;
			}
		};
		bool removeProperty(std::string key) {
			readProperties();
			if (testForProperty(key)) {
				properties.erase(key);
				flush();
				return true;
			} else {
				return false;
			}
		};
		bool testForProperty(std::string property) {
			if ( properties.find(property) == properties.end() ) {
				return false;
			} else {
				return true;
			}
		};
		void flush() {
			file.close();
			file.open(filename, std::ios::in|std::ios::out|std::ios::trunc);
			for (std::map<std::string,Property>::iterator it=properties.begin(); it!=properties.end(); ++it) {
				file << it->first << "=" << it->second.property << '\n';
			}
		};
	protected:
		size_t last_line;
		std::fstream file;
		std::string filename;
		std::map<std::string,Property> properties;
};
}
#endif /* SERVER_PROPERTIES_PARSER_H */
