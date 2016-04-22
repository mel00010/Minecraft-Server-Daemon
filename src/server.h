#ifndef SERVER_H
#define SERVER_H
#include "connection.h"
#include <string>
//~ class std::string;

class Server
{
	public:
		//~ Server(std::string screenSessionName);
		Server(
			std::string serverPath = "/home/minecraft", std::string serverJarName = "minecraft_server.jar", std::string serverAccount = "minecraft",
			int maxHeapAlloc = 2048, int minHeapAlloc = 1024, int gcThreadCount = 1, int historyLength = 1024, 
			std::string backupPath = "Backups",
			std::vector<std::string> worldsToBackup = {"world"},
			std::vector<std::string> javaArgs = {"XX:+UseConcMarkSweepGC","XX:+CMSIncrementalPacing","XX:+AggressiveOpts"}, 
			std::vector<std::string> serverOptions = {"nogui"}
		);
		virtual ~Server();
		void updateServer ();
		void backupServer ();
		void backupServer (std::string backupPath);
		void serverStatus ();
		void startServer   ();
		void stopServer   ();
		void restartServer();
		static void updateServer(std::string serverPath, std::string serverJarName, std::string serverAccount);
		static void backupServer(std::string serverPath, std::string serverAccount, std::string backupPath, std::vector<std::string> worldsToBackup);
	//~ protected:
		Connection connection;
};
#endif /* SERVER_H */

