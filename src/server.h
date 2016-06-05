#ifndef SERVER_H
#define SERVER_H
#include "ServerStream.h"
#include "log4cpp/Category.hh"
#include <string>
#include <time.h>
namespace MinecraftServerService {

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
		void listOnlinePlayers();
		void listOnlinePlayers(std::string playerName);
		void sendCommand(std::string command);
		void updateServer(std::string serverPath, std::string serverJarName, std::string serverAccount);
		void backupServer(std::string serverPath, std::string serverAccount, std::string backupPath, std::vector<std::string> worldsToBackup);
		ServerStream* getStream();
		std::string serverName;
	protected:
		//~ std::string getOutput(int timeout, int lines);
		void logger();
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
		std::iostream* serverProcess = nullptr;
		log4cpp::Category* log = nullptr;
};
}
#endif /* SERVER_H */

