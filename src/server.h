#ifndef SERVER_H
#define SERVER_H
#include "connection.h"
#include "log.h"
#include <string>
//~ class std::string;
extern Logger logger;

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
			std::vector<std::string> serverOptions
		);
		virtual ~Server();
		void updateServer();
		void backupServer();
		void backupServer(std::string backupPath);
		void serverStatus();
		void startServer();
		void stopServer();
		void restartServer();
		//~ void listOnlinePlayers();
		//~ void listOnlinePlayers(std::string playerName);
		void sendCommand(std::string command);
		static void updateServer(std::string serverPath, std::string serverJarName, std::string serverAccount);
		static void backupServer(std::string serverPath, std::string serverAccount, std::string backupPath, std::vector<std::string> worldsToBackup);
		Connection connection;
	protected:
		std::string serverName;
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
		//~ std::thread outputListener;
		//~ std::mutex mutex;
		//~ std::condition_variable cond;
		//~ bool stop = false;
		//~ Log log;
};
#endif /* SERVER_H */

