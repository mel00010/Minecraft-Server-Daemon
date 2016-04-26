#include "log.h"
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>

Logger::Logger()
{
	
}
Logger::Logger(std::string logDir, std::string logLevel)
{
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	std::strftime(buf, sizeof(buf), "%Y-%m-%d_%Hh%M", &tstruct);
	std::string time(buf);
	std::string filename =logDir + "/" +time + ".log";
	logFile.open(filename,std::ofstream::out);
}
void Logger::logToFile(std::string message, std::string logLevel)
{
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	std::strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M", &tstruct);
	std::string time(buf);
	std::cout << time << "\t" << logLevel << "\t" << message << std::endl;
	//~ logFile << time << "\t" << logLevel << "\t" << message << std::endl;
}
