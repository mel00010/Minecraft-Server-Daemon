#ifndef CONNECTION_H
#define CONNECTION_H
#include "log4cpp/Category.hh"
#include <string>
#include <vector>
#include <atomic>
#include <pstreams/pstream.h>
#include <memory>

class Connection
{
	public:
		//~ Connection(std::string _screenSessionName);
		Connection(log4cpp::Category& log);
		virtual ~Connection();
		void sendCommand(std::string command);
		std::shared_ptr<redi::pstream> startServer(std::string serverPath, std::string serverJarName, std::string serverAccount,
			int maxHeapAlloc, int minHeapAlloc, int gcThreadCount,
			std::vector<std::string> javaArgs, std::vector<std::string> serverOptions);
		void stopServer();
	protected:
		log4cpp::Category& log;
		std::shared_ptr<redi::pstream> server;
};
#endif /* CONNECTION_H */
