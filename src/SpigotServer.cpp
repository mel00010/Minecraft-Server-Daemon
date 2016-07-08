/*******************************************************************************
 *
 * Minecraft Server Daemon
 * Copyright (C) 2016  Mel McCalla <melmccalla@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 *
 *******************************************************************************/
 


#include "SpigotServer.hpp"

#include <log4cpp/Category.hh>
#include <unistd.h>
#include <ctime>

int system(const char *__command);

namespace MinecraftServerDaemon {
SpigotServer::SpigotServer( std::string serverName, std::string serverPath, std::string serverJarName, std::string serverAccount,
				int maxHeapAlloc, int minHeapAlloc, int gcThreadCount,
				std::string backupPath, std::vector<std::string> worldsToBackup, std::vector<std::string> javaArgs, 
				std::vector<std::string> serverOptions) 
:	serverName{serverName}, serverPath{serverPath}, serverJarName{serverJarName}, serverAccount{serverAccount},
				maxHeapAlloc{maxHeapAlloc}, minHeapAlloc{minHeapAlloc}, gcThreadCount{gcThreadCount}, backupPath{backupPath},
				worldsToBackup{worldsToBackup}, javaArgs{javaArgs}, serverOptions{serverOptions}, serverPropertiesParser("server.properties")
{
	log = &log4cpp::Category::getInstance(serverName);
	log->info(serverJarName);
	log->debug("SpigotServer::SpigotServer");
}
SpigotServer::~SpigotServer()
{
	log->debug("SpigotServer::~SpigotServer");
}
void SpigotServer::updateServer(std::string version)
{
	log->debug("SpigotServer::updateServer");
}
void SpigotServer::backupServer()
{
	log->debug("SpigotServer::backupServer");
	log->info("Starting backup");
	*this << "say SERVER BACKUP STARTING. Server going readonly..." << std::endl;
	*this << "save-off" << std::endl;
	*this << "save-all" << std::endl;
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
	*this << "save-on" << std::endl;
	*this << "say SERVER BACKUP ENDED. Server going read-write..." << std::endl;
	log->info("Backup finished");
}
void SpigotServer::backupServer(std::string _backupPath)
{
	log->debug("SpigotServer::backupServer");
	log->info("Starting backup");
	*this << "say SERVER BACKUP STARTING. Server going readonly..." << std::endl;
	*this << "save-off" << std::endl;
	*this << "save-all" << std::endl;
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
	*this << "save-on" << std::endl;
	*this << "say SERVER BACKUP ENDED. Server going read-write..." << std::endl;
	log->info("Backup finished");
}
void SpigotServer::reloadServer()
{
	*this << "reload" << std::endl;
}
void SpigotServer::startServer()
{
	log->debug("SpigotServer::startServer");
	if (!isRunning())
	{
		chdir(serverPath.c_str());
		launchServerProcess(serverPath, serverJarName, serverAccount, maxHeapAlloc, minHeapAlloc, gcThreadCount, javaArgs, serverOptions);
		log->debug("Launched server process");			
		std::thread outputListenerThread(&Server::outputListenerThread, serverPID, childProcessStdout[PIPE_READ], base, log, listeners);
		outputListenerThread.detach();
	}
}
void SpigotServer::stopServer()
{
	log->debug("SpigotServer::stopServer");
	if (isRunning())
	{
		*this << "say SERVER SHUTTING DOWN IN 10 SECONDS." << std::endl;
		for (int i=10; i>0; --i) {
			*this << "say "+std::to_string(i) << std::endl;
			sleep(1);
		}
		*this << "say Saving map..." << std::endl;
		*this << "save-all" << std::endl;
		*this << "stop" << std::endl;
		//~ sleep(10);
		while(isRunning()) {
			sleep(0.5);
		}
		log->info("Server stopped");
	} else {
		log->info("Server already stopped");
	}
}
void SpigotServer::serverStatus()
{
	log->debug("SpigotServer::serverStatus");
}
void SpigotServer::restartServer()
{
	log->debug("SpigotServer::restartServer");
	//~ if (isRunning() && !serverProcess->rdbuf()->exited())
	if (isRunning())
	{
		stopServer();
		startServer();
	}
}
void SpigotServer::sendCommand(std::string command)
{
	log->debug("SpigotServer::sendCommand");
	//~ if (isRunning() && !serverProcess->rdbuf()->exited())
	if (isRunning())
	{
		*this << command << std::endl;
	}
}

std::string SpigotServer::listOnlinePlayers()
{
	log->debug("SpigotServer::listOnlinePlayers");
	struct event* event;
	std::string* callbackOutput = new std::string;
	*callbackOutput = '\0';
	std::string output = '\0';
	Listener* listener = new Listener{callbackOutput, 10, false, 0, output};
	listeners->push_back(listener);
	*this << "list" << std::endl;
	while(*callbackOutput == "\0") { sleep(0.1); }
	if (callbackOutput->size() > 0) {
		std::string returnValue(*callbackOutput);
		delete callbackOutput;
		callbackOutput = nullptr;
		return returnValue;
	} else {
		log->info("No one is on the server");
		std::string returnValue = "No one is on the server";
		delete callbackOutput;
		callbackOutput = nullptr;
		return returnValue;
	}
}
bool SpigotServer::listOnlinePlayers(std::string playerName)
{
	log->debug("SpigotServer::listOnlinePlayers");
	struct event* event;
	std::string* callbackOutput = new std::string;
	*callbackOutput = '\0';
	std::string output = '\0';
	Listener* listener = new Listener{callbackOutput, 10, false, 0, output};
	listeners->push_back(listener);
	*this << "list" << std::endl;
	while(*callbackOutput == "\0") { sleep(0.1); }
	if (callbackOutput->size() > 0) {
		std::string returnValue(*callbackOutput);
		delete callbackOutput;
		callbackOutput = nullptr;
		return true;
	} else {
		log->info("No one is on the server");
		std::string returnValue = "No one is on the server";
		delete callbackOutput;
		callbackOutput = nullptr;
		return false;
	}
}
}
