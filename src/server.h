#ifndef SERVER_H
#define SERVER_H
#include "connection.h"
#include "log4cpp/Category.hh"
#include <string>
#include <time.h>

class Server
{
	public:
		Server(
			std::string serverName,
			std::string serverPath, std::string serverJarName, std::string serverAccount,
			int maxHeapAlloc, int minHeapAlloc, int gcThreadCount,
			std::string backupPath,
			std::vector<std::string> worldsToBackup,
			std::vector<std::string> javaArgs, 
			std::vector<std::string> serverOptions,
			struct event_base *base
		);
		virtual ~Server();
		void updateServer();
		void backupServer();
		void backupServer(std::string backupPath);
		void serverStatus();
		void startServer();
		void stopServer();
		void restartServer();
		void listOnlinePlayers();
		void listOnlinePlayers(std::string playerName);
		void sendCommand(std::string command);
		void updateServer(std::string serverPath, std::string serverJarName, std::string serverAccount);
		void backupServer(std::string serverPath, std::string serverAccount, std::string backupPath, std::vector<std::string> worldsToBackup);
		std::string serverName;
		volatile bool running = false;
	protected:
		std::string serverPath;
		std::string serverJarName;
		std::string serverAccount;
		int maxHeapAlloc;
		int minHeapAlloc;
		int gcThreadCount;
		std::string backupPath;
		std::vector<std::string> worldsToBackup;
		std::vector<std::string> javaArgs;
		std::vector<std::string> serverOptions;
		Connection* connection = nullptr;
		log4cpp::Category* log = nullptr;
		struct event_base *event_base;
};
#endif /* SERVER_H */

