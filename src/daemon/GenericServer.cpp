/*******************************************************************************
 *
 * Minecraft Server Daemon
 * GenericServer.cpp
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

#include <config.h>
#include <GenericServer.hpp>
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
 * @param worldsToBackup
 * @param javaArgs
 * @param serverOptions
 */
GenericServer::GenericServer(std::string serverName, std::string serverPath, std::string serverJarName, std::string serverAccount, int maxHeapAlloc,
		int minHeapAlloc, int gcThreadCount, std::string backupPath, std::vector<std::string> worldsToBackup, std::vector<std::string> javaArgs,
		std::vector<std::string> serverOptions) :
		serverName { serverName }, serverPath { serverPath }, serverJarName { serverJarName }, serverAccount { serverAccount }, maxHeapAlloc { maxHeapAlloc }, minHeapAlloc {
				minHeapAlloc }, gcThreadCount { gcThreadCount }, backupPath { backupPath }, worldsToBackup { worldsToBackup }, javaArgs { javaArgs }, serverOptions {
				serverOptions } {
	log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
	layout->setConversionPattern("%d %c [%p] %m%n ");

	log4cpp::Appender *serverFileAppender = new log4cpp::FileAppender(serverName, std::string(__LOG_DIR__) + "/servers/" + serverName + ".log");
	serverFileAppender->setLayout(layout);

	log = &log4cpp::Category::getInstance(std::string(serverName));
	log->addAppender(serverFileAppender);
	log->info(serverJarName);
	log->debug("GenericServer::GenericServer");
}
/**
 * Destructor.
 */
GenericServer::~GenericServer() {
	log->debug("GenericServer::~GenericServer");
}
/**
 * Currently unimplemented.
 * @param version
 */
void GenericServer::updateServer(__attribute__((unused))   std::string version) {
	log->debug("GenericServer::updateServer");
}
/**
 * Backs up the server.
 */
void GenericServer::backupServer() {
	log->debug("GenericServer::backupServer");
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
	for (std::string world : worldsToBackup) {
		log->info("Backing up " + world);
		std::string tarCommand = "tar -C \"" + serverPath + "\" -cf " + serverPath + "/" + backupPath + "/" + world + "_" + time + ".tar\" " + world;
		std::string gzipCommand = "gzip -f \"" + serverPath + "/" + backupPath + "/" + world + "_" + time + ".tar\"";
		log->info(tarCommand);
		system(tarCommand.c_str());
		log->info(gzipCommand);
		system(gzipCommand.c_str());
	}
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
void GenericServer::backupServer(std::string _backupPath) {
	log->debug("GenericServer::backupServer");
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
	for (std::string world : worldsToBackup) {
		log->info("Backing up " + world);
		std::string tarCommand = "tar -C \"" + serverPath + "\" -cf " + _backupPath + "/" + world + "_" + time + ".tar\" " + world;
		std::string gzipCommand = "gzip -f \"" + backupPath + "/" + world + "_" + time + ".tar\"";
		log->info(tarCommand);
		system(tarCommand.c_str());
		log->info(gzipCommand);
		system(gzipCommand.c_str());
	}
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
 * Starts the server.
 */
void GenericServer::startServer() {
	log->debug("GenericServer::startServer");
	if (!isRunning()) {
		chdir(serverPath.c_str());
		launchServerProcess(serverPath, serverJarName, serverAccount, maxHeapAlloc, minHeapAlloc, gcThreadCount, javaArgs, serverOptions);
		log->debug("Launched server process");
		std::thread outputListenerThread(&Server::outputListenerThread, serverPID, serverName, this, childProcessStdout[PIPE_READ], base, log, listeners,
				players);
		outputListenerThread.detach();
	}
}
/**
 * Stops the server.
 */
void GenericServer::stopServer() {
	log->debug("GenericServer::stopServer");
	if (isRunning()) {
		*this << "say SERVER SHUTTING DOWN IN 10 SECONDS." << std::endl;
		for (int i = 10; i > 0; --i) {
			*this << "say " + std::to_string(i) << std::endl;
			sleep(1);
		}
		*this << "say Saving map..." << std::endl;
		*this << "save-all" << std::endl;
		*this << "stop" << std::endl;
		//~ sleep(10);
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
void GenericServer::serverStatus() {
	log->debug("GenericServer::serverStatus");
}
/**
 * Restarts the server.
 */
void GenericServer::restartServer() {
	log->debug("GenericServer::restartServer");
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
void GenericServer::sendCommand(std::string command) {
	log->debug("GenericServer::sendCommand");
	//~ if (isRunning() && !serverProcess->rdbuf()->exited())
	if (isRunning()) {
		*this << command << std::endl;
	}
}

} /* namespace MinecraftServerDaemon */
