#ifndef CONNECTION_H
#define CONNECTION_H
#include "log.h"
#include <string>
#include <vector>
#include <atomic>
extern Logger logger;

class Connection
{
	public:
		//~ Connection(std::string _screenSessionName);
		Connection();
		virtual ~Connection();
		void sendCommand(std::string command);
		int startServer(std::string serverPath, std::string serverJarName, std::string serverAccount,
			int maxHeapAlloc, int minHeapAlloc, int gcThreadCount,
			std::vector<std::string> javaArgs, std::vector<std::string> serverOptions);
		void stopServer();
		void listenForOutput();
		int serverstdin[2];
		int serverstdout[2];
	protected:
		int server;
		//~ Log log;
		//~ std::atomic<bool> stopped;
};
#endif /* CONNECTION_H */
