#include "parser.h"
#include <iostream>
#include <json/json.h>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>  

//~ Parser::Parser()
//~ {
//~ }
Json::Value Parser::parse(std::string configFile)
{
	Json::Value root;
	Json::Reader reader;
	try 
	{
		reader.parse(getFileContents(configFile.c_str()),root);	
	}
	catch (std::bad_alloc& e) 
	{
		std::cout << "\033[1mError reading file " << configFile << "\033[0m"<< std::endl;
		exit(1);
	}
	return root;
}

std::string Parser::getFileContents(const char *filename)
{
	std::ifstream in(filename, std::ios::in | std::ios::binary);	
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	} else {
		std::cout << "\033[1mError File " << filename << " not found\033[0m" << std::endl;
		exit(1);
	}
	//~ throw(errno);
}
//~ Parser::~Parser()
//~ {
//~ }

