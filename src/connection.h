#ifndef CONNECTION_H
#define CONNECTION_H
#include <string>
#include <vector>
class Connection
{
	public:
		//~ Connection(std::string _screenSessionName);
		Connection(
			std::string _serverPath, std::string _serverJarName, std::string _serverAccount,
			int _maxHeapAlloc, int _minHeapAlloc, int _gcThreadCount, int _historyLength, 
			std::string _backupPath, std::vector<std::string> _worldsToBackup,std::vector<std::string> _javaArgs, std::vector<std::string> _serverOptions
		);
		virtual ~Connection();
		std::stringstream sendCommand(std::string command);
		void listOnlinePlayers();
		void listOnlinePlayers(std::string playerName);
		int startServer();
		void stopServer();
		std::vector<std::string> worldsToBackup;
		std::string backupPath;
		std::string screenSessionName;
		std::string serverPath;
		std::string serverJarName;
		std::string serverAccount;
		int serverstdin[2];
		int serverstdout[2];
	protected:
		std::vector<std::string> javaArgs;
		std::vector<std::string> serverOptions;
		int maxHeapAlloc;
		int minHeapAlloc;
		int gcThreadCount;
		int historyLength;
		int server;
};
#endif /* CONNECTION_H */
