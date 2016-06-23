#ifndef VANILLA_SERVER_H
#define VANILLA_SERVER_H
#include "server.h"
#include "outputListener.h"
#include "ServerStream.h"
#include "log4cpp/Category.hh"
#include <string>
#include <sstream>
#include <vector>
#include <thread>
#include <time.h>
namespace MinecraftServerService {
class VanillaServer : public Server
{
	public:
		VanillaServer(
			std::string serverName,
			std::string serverPath, std::string serverJarName, std::string serverAccount,
			int maxHeapAlloc, int minHeapAlloc, int gcThreadCount,
			std::string backupPath,
			std::vector<std::string> worldsToBackup,
			std::vector<std::string> javaArgs, 
			std::vector<std::string> serverOptions
		);
		virtual ~VanillaServer();
		void updateServer();
		void backupServer();
		void backupServer(std::string backupPath);
		void serverStatus();
		void startServer();
		void stopServer();
		void restartServer();
		void reloadServer() {};
		std::string listOnlinePlayers();
		void listOnlinePlayers(std::string playerName);
		void sendCommand(std::string command);
		std::string serverName;
	protected:
		void logger(size_t linesRequested, std::stringstream* output, log4cpp::Category* log);
		void listOnlinePlayersCallback(size_t linesRequested, std::stringstream* output, log4cpp::Category* log);
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
		std::stringstream* callbackOutput = nullptr;
};
}
#endif /* VANILLA_SERVER_H */
