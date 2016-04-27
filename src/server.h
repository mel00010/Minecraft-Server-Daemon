#ifndef SERVER_H
#define SERVER_H
#include "connection.h"
#include "log4cpp/Category.hh"
#include <string>
#include <thread>
#include <memory>


class Server
{
	public:
		Server(
			//~ Log log,
			std::string serverName,
			std::string serverPath, std::string serverJarName, std::string serverAccount,
			int maxHeapAlloc, int minHeapAlloc, int gcThreadCount,
			std::string backupPath,
			std::vector<std::string> worldsToBackup,
			std::vector<std::string> javaArgs, 
			std::vector<std::string> serverOptions,
			log4cpp::Category& log
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
		static void updateServer(std::string serverPath, std::string serverJarName, std::string serverAccount);
		static void backupServer(std::string serverPath, std::string serverAccount, std::string backupPath, std::vector<std::string> worldsToBackup);
		std::string serverName;
	protected:
		void outputListener(std::shared_ptr<redi::pstream> server);
		
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
		Connection connection;
		log4cpp::Category& log;
		//~ std::shared_ptr<redi::pstream> server;
};
#endif /* SERVER_H */

