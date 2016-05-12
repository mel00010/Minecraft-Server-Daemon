#include "server.h"
#include "log4cpp/Category.hh"
#include <log4cpp/PropertyConfigurator.hh>
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
				std::vector<std::string> _serverOptions)
{
	
	running = false;
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
	//~ log4cpp::PropertyConfigurator::configure("/etc/minecraft/log4cpp.properties");
	log = &log4cpp::Category::getInstance(std::string("server"));
	log->info(serverJarName);
	log->debug("Server::Server");
}
Server::~Server()
{
	log->debug("Server::~Server");
}
void Server::updateServer()
{
	log->debug("Server::updateServer");
}
void Server::updateServer(std::string serverPath, std::string serverJarName, std::string serverAccount)
{
	log->debug("Server::updateServer");
}
void Server::backupServer()
{
	log->debug("Server::backupServer");
	log->info("Starting backup");
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
		log->info("Backing up "+ world);
		std::string tarCommand = "tar -C \""+serverPath+"\" -cf "+serverPath+"/"+backupPath+"/"+world+"_"+time+".tar\" "+world;
		std::string gzipCommand = "gzip -f \""+serverPath+"/"+backupPath+"/"+world+"_"+time+".tar\"";
		log->info(tarCommand);
		system(tarCommand.c_str());
		log->info(gzipCommand);
		system(gzipCommand.c_str());
	}
	log->info("Backing up "+serverJarName);
	std::string copyJarCommand = "cp \""+serverPath+"/"+serverJarName+"\" \""+backupPath+"/"+serverJarName.substr(0, serverJarName.size()-4)+"_"+time+".jar\"";
	log->info(copyJarCommand);
	system(copyJarCommand.c_str());
	sendCommand("save-on");
	sendCommand("say SERVER BACKUP ENDED. Server going read-write...");
	log->info("Backup finished");
}
void Server::backupServer(std::string _backupPath)
{
	log->debug("Server::backupServer");
	log->info("Starting backup");
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
		log->info("Backing up "+world);
		std::string tarCommand = "tar -C \""+serverPath+"\" -cf "+_backupPath+"/"+world+"_"+time+".tar\" "+world;
		std::string gzipCommand = "gzip -f \""+backupPath+"/"+world+"_"+time+".tar\"";
		log->info(tarCommand);
		system(tarCommand.c_str());
		log->info(gzipCommand);
		system(gzipCommand.c_str());
	}
	log->info("Backing up "+serverJarName);
	std::string copyJarCommand = "cp \""+serverPath+"/"+serverJarName+"\" \""+_backupPath+"/"+serverJarName.substr(0, serverJarName.size()-4)+"_"+time+".jar\"";
	log->info(copyJarCommand);
	system(copyJarCommand.c_str());
	sendCommand("save-on");
	sendCommand("say SERVER BACKUP ENDED. Server going read-write...");
	log->info("Backup finished");
}
void Server::backupServer(std::string _serverPath, std::string _serverAccount, std::string _backupPath, std::vector<std::string> _worldsToBackup)
{
	log->debug("Server::backupServer");
}
void Server::startServer()
{
	log->debug("Server::startServer");
	if (!running)
	{
		Connection* _connection = new Connection(serverName);
		connection = _connection;
		connection->startServer(serverPath, serverJarName, serverAccount, maxHeapAlloc, minHeapAlloc, gcThreadCount, javaArgs, serverOptions);
	}
	running = true;
}
void Server::stopServer()
{
	log->debug("Server::stopServer");
	if (running)
	{
		connection->stopServer();
		delete(connection);
		connection = NULL;
	} else {
		log->info("Server already stopped");
	}
	running = false;
}
void Server::serverStatus()
{
	log->debug("Server::serverStatus");
}
void Server::restartServer()
{
	log->debug("Server::restartServer");
	if (running)
	{
		stopServer();
		startServer();
	}
}
void Server::sendCommand(std::string command)
{
	log->debug("Server::sendCommand");
	if (running)
	{
		connection->sendCommand(command);
	}
}

void Server::listOnlinePlayers()
{
	log->debug("Server::listOnlinePlayers");
	
	
	//~ std::stringstream output;
	//~ std::string line;
	//~ int numPlayers = 0;
	
	//~ std::stringstream playerList = connection->sendCommand("list");
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
		//~ log->info(outputBuf);
	//~ } else {
		//~ log->info("No one is on the server");
	//~ }
}
void Server::listOnlinePlayers(std::string playerName)
{
	//~ std::stringstream output;
	//~ std::string line;
	//~ int numPlayers = 0;
	
	//~ std::stringstream playerList = connection->sendCommand("list");
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
		//~ log->info(outputBuf);
	//~ } else {
		//~ log->info("No one is on the server");
	//~ }
}
