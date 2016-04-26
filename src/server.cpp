#include "server.h"
#include "log.h"
#include "connection.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
//~ #include <thread>
#include <time.h>
#include <ctime>

Server::Server( std::string _serverName, std::string _serverPath, std::string _serverJarName, std::string _serverAccount,
				int _maxHeapAlloc, int _minHeapAlloc, int _gcThreadCount,
				std::string _backupPath, std::vector<std::string> _worldsToBackup, std::vector<std::string> _javaArgs, 
				std::vector<std::string> _serverOptions)
{
	serverPath = _serverPath;
	serverJarName = _serverJarName;
	serverAccount = _serverAccount;
	maxHeapAlloc = _maxHeapAlloc;
	minHeapAlloc = _minHeapAlloc;
	gcThreadCount = _gcThreadCount;
	backupPath = _backupPath;
	worldsToBackup = _worldsToBackup;
	javaArgs = _javaArgs;
	serverOptions = _serverOptions;
	logger.logToFile(serverJarName,"INFO");
	logger.logToFile("End of server constructor", "INFO");
}
Server::~Server()
{
}
void Server::updateServer()
{
	
}
void Server::updateServer(std::string serverPath, std::string serverJarName, std::string serverAccount)
{
	
}
void Server::backupServer()
{
	logger.logToFile("Starting backup", "NOTICE");
	connection.sendCommand("say SERVER BACKUP STARTING. Server going readonly...");
	connection.sendCommand("save-off");
	connection.sendCommand("save-all");
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	std::strftime(buf, sizeof(buf), "%Y-%m-%d_%Hh%M", &tstruct);
	std::string time(buf);
	for (std::string world : worldsToBackup)
	{
		logger.logToFile("Backing up "+ world, "NOTICE");
		std::string tarCommand = "tar -C \""+serverPath+"\" -cf "+serverPath+"/"+backupPath+"/"+world+"_"+time+".tar\" "+world;
		std::string gzipCommand = "gzip -f \""+serverPath+"/"+backupPath+"/"+world+"_"+time+".tar\"";
		logger.logToFile(tarCommand, "NOTICE");
		system(tarCommand.c_str());
		logger.logToFile(gzipCommand, "NOTICE");
		system(gzipCommand.c_str());
	}
	logger.logToFile("Backing up "+serverJarName, "NOTICE");
	std::string copyJarCommand = "cp \""+serverPath+"/"+serverJarName+"\" \""+backupPath+"/"+serverJarName.substr(0, serverJarName.size()-4)+"_"+time+".jar\"";
	logger.logToFile(copyJarCommand, "NOTICE");
	system(copyJarCommand.c_str());
	connection.sendCommand("save-on");
	connection.sendCommand("say SERVER BACKUP ENDED. Server going read-write...");
	logger.logToFile("Backup finished", "NOTICE");
}
void Server::backupServer(std::string _backupPath)
{
	logger.logToFile("Starting backup", "NOTICE");
	connection.sendCommand("say SERVER BACKUP STARTING. Server going readonly...");
	connection.sendCommand("save-off");
	connection.sendCommand("save-all");
	time_t now = time(0);

	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	std::strftime(buf, sizeof(buf), "%Y-%m-%d_%Hh%M", &tstruct);
	std::string time(buf);
	for (std::string world : worldsToBackup)
	{
		logger.logToFile("Backing up "+world, "NOTICE");
		std::string tarCommand = "tar -C \""+serverPath+"\" -cf "+_backupPath+"/"+world+"_"+time+".tar\" "+world;
		std::string gzipCommand = "gzip -f \""+backupPath+"/"+world+"_"+time+".tar\"";
		logger.logToFile(tarCommand, "NOTICE");
		system(tarCommand.c_str());
		logger.logToFile(gzipCommand, "NOTICE");
		system(gzipCommand.c_str());
	}
	logger.logToFile("Backing up "+serverJarName, "NOTICE");
	std::string copyJarCommand = "cp \""+serverPath+"/"+serverJarName+"\" \""+_backupPath+"/"+serverJarName.substr(0, serverJarName.size()-4)+"_"+time+".jar\"";
	logger.logToFile(copyJarCommand, "NOTICE");
	system(copyJarCommand.c_str());
	connection.sendCommand("save-on");
	connection.sendCommand("say SERVER BACKUP ENDED. Server going read-write...");
	logger.logToFile("Backup finished", "NOTICE");
}
void Server::backupServer(std::string _serverPath, std::string _serverAccount, std::string _backupPath, std::vector<std::string> _worldsToBackup)
{
	
}
void Server::startServer()
{
	connection.startServer(serverPath, serverJarName, serverAccount, maxHeapAlloc, minHeapAlloc, gcThreadCount, javaArgs, serverOptions);
	//~ std::thread listenForOutput(&Connection::listenForOutput, connection);
	//~ connection.listenForOutput();
}
void Server::stopServer()
{
	connection.stopServer();
}
void Server::serverStatus()
{
	
}
void Server::restartServer()
{
	connection.stopServer();
	connection.startServer(serverPath, serverJarName, serverAccount, maxHeapAlloc, minHeapAlloc, gcThreadCount, javaArgs, serverOptions);
}
void Server::sendCommand(std::string command)
{
	connection.sendCommand(command);
}
//~ void Server::listOnlinePlayers()
//~ {
	
	//~ std::stringstream output;
	//~ std::string line;
	//~ int numPlayers = 0;
	
	//~ std::stringstream playerList = connection.sendCommand("list");
	//~ std::stringbuf *listBuf = playerList.rdbuf();
	
	//~ std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	//~ while (listBuf->in_avail() != 0)
	//~ {
		//~ getline(playerList, line);
		//~ output << line << std::endl;
		//~ numPlayers++;
	//~ }
	//~ if (numPlayers != 0) {
		//~ std::string outputBuf = output.str();
		//~ logger.logToFile(outputBuf, "NOTICE");
	//~ } else {
		//~ logger.logToFile("No one is on the server", "NOTICE");
	//~ }
//~ }
//~ void Server::listOnlinePlayers(std::string playerName)
//~ {
	//~ std::stringstream output;
	//~ std::string line;
	//~ int numPlayers = 0;
	
	//~ std::stringstream playerList = connection.sendCommand("list");
	//~ std::stringbuf *listBuf = playerList.rdbuf();
	
	//~ std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	//~ while (listBuf->in_avail() != 0)
	//~ {
		//~ getline(playerList, line);
		//~ if (line != playerName) {
			//~ output << line << std::endl;
			//~ numPlayers++;
		//~ }
	//~ }
	//~ if (numPlayers != 0) {
		//~ std::string outputBuf = output.str();
		//~ logger.logToFile(outputBuf, "NOTICE");
	//~ } else {
		//~ logger.logToFile("No one is on the server", "NOTICE");
	//~ }
//~ }
