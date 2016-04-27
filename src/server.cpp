#include "server.h"
#include "log4cpp/Category.hh"
#include "connection.h"
#include <pstreams/pstream.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <memory>
#include <future>
#include <chrono>
#include <time.h>
#include <ctime>


Server::Server( std::string _serverName, std::string _serverPath, std::string _serverJarName, std::string _serverAccount,
				int _maxHeapAlloc, int _minHeapAlloc, int _gcThreadCount,
				std::string _backupPath, std::vector<std::string> _worldsToBackup, std::vector<std::string> _javaArgs, 
				std::vector<std::string> _serverOptions, log4cpp::Category& log) : log{log}, connection(log)
{
	serverName = _serverName;
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
	//~ log = _log;
	log.info(serverJarName);
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
	log.info("Starting backup");
	sendCommand("say SERVER BACKUP STARTING. Server going readonly...");
	sendCommand("save-off");
	sendCommand("save-all");
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	std::strftime(buf, sizeof(buf), "%Y-%m-%d_%Hh%M", &tstruct);
	std::string time(buf);
	for (std::string world : worldsToBackup)
	{
		log.info("Backing up "+ world);
		std::string tarCommand = "tar -C \""+serverPath+"\" -cf "+serverPath+"/"+backupPath+"/"+world+"_"+time+".tar\" "+world;
		std::string gzipCommand = "gzip -f \""+serverPath+"/"+backupPath+"/"+world+"_"+time+".tar\"";
		log.info(tarCommand);
		system(tarCommand.c_str());
		log.info(gzipCommand);
		system(gzipCommand.c_str());
	}
	log.info("Backing up "+serverJarName);
	std::string copyJarCommand = "cp \""+serverPath+"/"+serverJarName+"\" \""+backupPath+"/"+serverJarName.substr(0, serverJarName.size()-4)+"_"+time+".jar\"";
	log.info(copyJarCommand);
	system(copyJarCommand.c_str());
	sendCommand("save-on");
	sendCommand("say SERVER BACKUP ENDED. Server going read-write...");
	log.info("Backup finished");
}
void Server::backupServer(std::string _backupPath)
{
	log.info("Starting backup");
	sendCommand("say SERVER BACKUP STARTING. Server going readonly...");
	sendCommand("save-off");
	sendCommand("save-all");
	time_t now = time(0);

	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	std::strftime(buf, sizeof(buf), "%Y-%m-%d_%Hh%M", &tstruct);
	std::string time(buf);
	for (std::string world : worldsToBackup)
	{
		log.info("Backing up "+world);
		std::string tarCommand = "tar -C \""+serverPath+"\" -cf "+_backupPath+"/"+world+"_"+time+".tar\" "+world;
		std::string gzipCommand = "gzip -f \""+backupPath+"/"+world+"_"+time+".tar\"";
		log.info(tarCommand);
		system(tarCommand.c_str());
		log.info(gzipCommand);
		system(gzipCommand.c_str());
	}
	log.info("Backing up "+serverJarName);
	std::string copyJarCommand = "cp \""+serverPath+"/"+serverJarName+"\" \""+_backupPath+"/"+serverJarName.substr(0, serverJarName.size()-4)+"_"+time+".jar\"";
	log.info(copyJarCommand);
	system(copyJarCommand.c_str());
	sendCommand("save-on");
	sendCommand("say SERVER BACKUP ENDED. Server going read-write...");
	log.info("Backup finished");
}
void Server::backupServer(std::string _serverPath, std::string _serverAccount, std::string _backupPath, std::vector<std::string> _worldsToBackup)
{
	
}
void Server::startServer()
{
	std::shared_ptr<redi::pstream> server = connection.startServer(serverPath, serverJarName, serverAccount, maxHeapAlloc, minHeapAlloc, gcThreadCount, javaArgs, serverOptions);
	std::thread outputListener(&Server::outputListener, this, server);
	outputListener.detach();
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
	std::shared_ptr<redi::pstream> server = connection.startServer(serverPath, serverJarName, serverAccount, maxHeapAlloc, minHeapAlloc, gcThreadCount, javaArgs, serverOptions);
	std::thread outputListener(&Server::outputListener, this, server);
	outputListener.detach();
}
void Server::sendCommand(std::string command)
{
	connection.sendCommand(command);
}
void Server::outputListener(std::shared_ptr<redi::pstream> _server)
{
	//~ log4cpp::Category& __log = _log
	std::string output;
	while (std::getline(*_server, output) && !(_server->rdbuf()->exited())) {
		log.info(output);
	}
}
void Server::listOnlinePlayers()
{
	
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
		//~ log.info(outputBuf);
	//~ } else {
		//~ log.info("No one is on the server");
	//~ }
}
void Server::listOnlinePlayers(std::string playerName)
{
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
		//~ log.info(outputBuf);
	//~ } else {
		//~ log.info("No one is on the server");
	//~ }
}
