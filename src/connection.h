#ifndef CONNECTION_H
#define CONNECTION_H
#include "log4cpp/Category.hh"
#include <string>
#include <vector>
#include <pstreams/pstream.h>
#include <memory>

class Connection
{
	public:
		//~ Connection(std::string _screenSessionName);
		//~ Connection();
		Connection(std::string _serverName, struct event_base *_event_base);
		virtual ~Connection();
		void sendCommand(std::string command);
		void startServer(std::string serverPath, std::string serverJarName, std::string serverAccount,
			int maxHeapAlloc, int minHeapAlloc, int gcThreadCount,
			std::vector<std::string> javaArgs, std::vector<std::string> serverOptions);
		void stopServer();
	protected:
		void outputListener();
		log4cpp::Category* log = nullptr;
		std::shared_ptr<redi::pstream> server;
		std::string serverName;
		volatile bool running = false;
		struct event_base *event_base;
};
#endif /* CONNECTION_H */
