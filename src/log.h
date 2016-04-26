#ifndef LOG_H
#define LOG_H
#include <string>
#include <fstream>
class Logger
{
	public:
		Logger();
		Logger(std::string logDir, std::string logLevel);
		void logToFile(std::string message, std::string logLevel);
		std::ofstream logFile;
		std::string logLevel;
};
#endif /* LOG_H */
