#include "server.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <time.h>
#include <ctime>

Server::Server(std::string screenSessionName) : connection(screenSessionName)
{
	
}
Server::Server(	std::string screenSessionName, std::string serverPath, std::string serverJarName, std::string serverAccount,
				int maxHeapAlloc, int minHeapAlloc, int gcThreadCount, int historyLength, 
				std::string backupPath, std::vector<std::string> worldsToBackup, std::vector<std::string> javaArgs, 
				std::vector<std::string> serverOptions ) 
:	connection(	screenSessionName, serverPath, serverJarName, serverAccount, 
				maxHeapAlloc, minHeapAlloc, gcThreadCount, historyLength, backupPath, worldsToBackup, 
				javaArgs, serverOptions )
{
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
	std::cout << "Starting backup" << std::endl;
	connection.sendCommand("say SERVER BACKUP STARTING. Server going readonly...");
	connection.sendCommand("save-off");
	connection.sendCommand("save-all");
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	std::strftime(buf, sizeof(buf), "%Y-%m-%d_%Hh%M", &tstruct);
	std::string time(buf);
	for (std::string world : connection.worldsToBackup)
	{
		std::cout << "Backing up " << world << std::endl;
		std::string tarCommand = "tar -C \""+connection.serverPath+"\" -cf "+connection.serverPath+"/"+connection.backupPath+"/"+world+"_"+time+".tar\" "+world;
		std::string gzipCommand = "gzip -f \""+connection.serverPath+"/"+connection.backupPath+"/"+world+"_"+time+".tar\"";
		std::cout << tarCommand << std::endl;
		system(tarCommand.c_str());
		std::cout << gzipCommand << std::endl;
		system(gzipCommand.c_str());
	}
	std::cout << "Backing up " << connection.serverJarName << std::endl;
	std::string copyJarCommand = "cp \""+connection.serverPath+"/"+connection.serverJarName+"\" \""+connection.backupPath+"/"+connection.serverJarName.substr(0, connection.serverJarName.size()-4)+"_"+time+".jar\"";
	std::cout << copyJarCommand << std::endl;
	system(copyJarCommand.c_str());
	connection.sendCommand("save-on");
	connection.sendCommand("say SERVER BACKUP ENDED. Server going read-write...");
	std::cout << "Backup finished" << std::endl;
}
void Server::backupServer(std::string backupPath)
{
	std::cout << "Starting backup" << std::endl;
	connection.sendCommand("say SERVER BACKUP STARTING. Server going readonly...");
	connection.sendCommand("save-off");
	connection.sendCommand("save-all");
	time_t now = time(0);

	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	std::strftime(buf, sizeof(buf), "%Y-%m-%d_%Hh%M", &tstruct);
	std::string time(buf);
	for (std::string world : connection.worldsToBackup)
	{
		std::cout << "Backing up " << world << std::endl;
		std::string tarCommand = "tar -C \""+connection.serverPath+"\" -cf "+backupPath+"/"+world+"_"+time+".tar\" "+world;
		std::string gzipCommand = "gzip -f \""+backupPath+"/"+world+"_"+time+".tar\"";
		std::cout << tarCommand << std::endl;
		system(tarCommand.c_str());
		std::cout << gzipCommand << std::endl;
		system(gzipCommand.c_str());
	}
	std::cout << "Backing up " << connection.serverJarName << std::endl;
	std::string copyJarCommand = "cp \""+connection.serverPath+"/"+connection.serverJarName+"\" \""+backupPath+"/"+connection.serverJarName.substr(0, connection.serverJarName.size()-4)+"_"+time+".jar\"";
	std::cout << copyJarCommand << std::endl;
	system(copyJarCommand.c_str());
	connection.sendCommand("save-on");
	connection.sendCommand("say SERVER BACKUP ENDED. Server going read-write...");
	std::cout << "Backup finished" << std::endl;
}
void Server::backupServer(std::string serverPath, std::string serverAccount, std::string backupPath, std::vector<std::string> worldsToBackup)
{
	
}
void Server::startServer()
{
	connection.startServer();
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
	connection.startServer();
	connection.stopServer();
}
