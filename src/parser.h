#ifndef PARSER_H
#define PARSER_H
#include <stdlib.h>
#include <string>
#include <json/json.h>
class Parser
{
	public:
		//~ Parser();
		//~ virtual ~Parser();
		Json::Value parse(std::string configFile);
	protected:
		
		std::string getFileContents(const char *filename);
};
#endif //PARSER_H

