/*******************************************************************************
 *
 * Minecraft Server Daemon
 * BungeeCordServer.cpp
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

#include <BungeeCordServer.hpp>
#include <config.h>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <thread>

namespace MinecraftServerDaemon {
/**
 * Constructor.
 * @param serverName
 * @param serverPath
 * @param serverJarName
 * @param serverAccount
 * @param maxHeapAlloc
 * @param minHeapAlloc
 * @param gcThreadCount
 * @param backupPath
 * @param javaArgs
 * @param serverOptions
 */
BungeeCordServer::BungeeCordServer(std::string serverName, std::string serverPath, std::string serverJarName, std::string serverAccount, int maxHeapAlloc,
		int minHeapAlloc, int gcThreadCount, std::string backupPath, std::vector<std::string> javaArgs, std::vector<std::string> serverOptions) :
		serverName { serverName }, serverPath { serverPath }, serverJarName { serverJarName }, serverAccount { serverAccount }, maxHeapAlloc { maxHeapAlloc }, minHeapAlloc {
				minHeapAlloc }, gcThreadCount { gcThreadCount }, backupPath { backupPath }, javaArgs { javaArgs }, serverOptions { serverOptions } {
	log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
	layout->setConversionPattern("%d %c [%p] %m%n ");

	log4cpp::Appender *serverFileAppender = new log4cpp::FileAppender(serverName, std::string(__LOG_DIR__) + "/servers/" + serverName + ".log");
	serverFileAppender->setLayout(layout);

	log = &log4cpp::Category::getInstance(std::string(serverName));
	log->addAppender(serverFileAppender);
	log->info(serverJarName);
	log->debug("BungeeCordServer::BungeeCordServer");
}
/**
 * Destructor.
 */
BungeeCordServer::~BungeeCordServer() {
	log->debug("BungeeCordServer::~BungeeCordServer");
}
/**
 * Currently unimplemented.
 * @param version
 */
void BungeeCordServer::updateServer(__attribute__((unused))     std::string version) {
	log->debug("BungeeCordServer::updateServer");
}
/**
 * Backs up the server.
 */
void BungeeCordServer::backupServer() {
	log->debug("BungeeCordServer::backupServer");
	log->info("Starting backup");
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	std::strftime(buf, sizeof(buf), "%Y-%m-%d_%Hh%M", &tstruct);
	std::string time(buf);
	log->info("Backing up " + serverJarName);
	std::string copyJarCommand = "cp \"" + serverPath + "/" + serverJarName + "\" \"" + backupPath + "/" + serverJarName.substr(0, serverJarName.size() - 4)
			+ "_" + time + ".jar\"";
	log->info(copyJarCommand);
	system(copyJarCommand.c_str());
	*this << "save-on" << std::endl;
	*this << "say SERVER BACKUP ENDED. Server going read-write..." << std::endl;
	log->info("Backup finished");
}
/**
 * Backs up the server to a specified path.
 * @param _backupPath
 */
void BungeeCordServer::backupServer(std::string _backupPath) {
	log->debug("BungeeCordServer::backupServer");
	log->info("Starting backup");
	time_t now = time(0);

	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	std::strftime(buf, sizeof(buf), "%Y-%m-%d_%Hh%M", &tstruct);
	std::string time(buf);
	log->info("Backing up " + serverJarName);
	std::string copyJarCommand = "cp \"" + serverPath + "/" + serverJarName + "\" \"" + _backupPath + "/" + serverJarName.substr(0, serverJarName.size() - 4)
			+ "_" + time + ".jar\"";
	log->info(copyJarCommand);
	system(copyJarCommand.c_str());
	*this << "save-on" << std::endl;
	*this << "say SERVER BACKUP ENDED. Server going read-write..." << std::endl;
	log->info("Backup finished");
}
/**
 * Reloads the server.
 */
void BungeeCordServer::reloadServer() {
	*this << "greload" << std::endl;
}
/**
 * Starts the server.
 */
void BungeeCordServer::startServer() {
	log->debug("BungeeCordServer::startServer");
	if (!isRunning()) {
		chdir(serverPath.c_str());
		launchServerProcess(serverPath, serverJarName, serverAccount, maxHeapAlloc, minHeapAlloc, gcThreadCount, javaArgs, serverOptions);
		log->debug("Launched server process");
		std::thread outputListenerThread(&Server::outputListenerThread, serverPID, childProcessStdout[PIPE_READ], base, log, listeners);
		outputListenerThread.detach();
	}
}
/**
 * Stops the server.
 */
void BungeeCordServer::stopServer() {
	log->debug("BungeeCordServer::stopServer");
	if (isRunning()) {
		*this << "end" << std::endl;
		while (isRunning()) {
			sleep(0.5);
		}
		log->info("Server stopped");
	} else {
		log->info("Server already stopped");
	}
}
/**
 * Currently unimplemented.
 */
void BungeeCordServer::serverStatus() {
	log->debug("BungeeCordServer::serverStatus");
}
/**
 * Restarts the server.
 */
void BungeeCordServer::restartServer() {
	log->debug("BungeeCordServer::restartServer");
	//~ if (isRunning() && !serverProcess->rdbuf()->exited())
	if (isRunning()) {
		stopServer();
		startServer();
	}
}
/**
 * Sends the specified command to the server.
 * @param command
 */
void BungeeCordServer::sendCommand(std::string command) {
	log->debug("BungeeCordServer::sendCommand");
	//~ if (isRunning() && !serverProcess->rdbuf()->exited())
	if (isRunning()) {
		*this << command << std::endl;
	}
}
/**
 * Lists the players on the server.
 * @return
 */
std::string BungeeCordServer::listOnlinePlayers() {
	log->debug("BungeeCordServer::listOnlinePlayers");
	std::string* callbackOutput = new std::string;
	*callbackOutput = '\0';
	std::string output = '\0';
	Listener* listener = new Listener { callbackOutput, 10, false, 0, output };
	listeners->push_back(listener);
	*this << "glist" << std::endl;
	while (*callbackOutput == "\0") {
		sleep(0.1);
	}
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
/**
 * Tests if the specified player is on the server.  (Currently non functional)
 * @param playerName
 * @return
 */
bool BungeeCordServer::listOnlinePlayers(__attribute__((unused))  std::string playerName) {
	log->debug("BungeeCordServer::listOnlinePlayers");
	std::string* callbackOutput = new std::string;
	*callbackOutput = '\0';
	std::string output = '\0';
	Listener* listener = new Listener { callbackOutput, 10, false, 0, output };
	listeners->push_back(listener);
	*this << "glist" << std::endl;
	while (*callbackOutput == "\0") {
		sleep(0.1);
	}
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

} /* namespace MinecraftServerDaemon */
