/*******************************************************************************
 *
 * Minecraft Server Daemon
 * VanillaServer.cpp
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
#include <curl/curl.h>
#include <curl/easy.h>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <unistd.h>
#include <VanillaServer.hpp>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <thread>

namespace MinecraftServerDaemon {
/**
 * Initializes the class members and sets up logging.
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
VanillaServer::VanillaServer(std::string serverName, std::string serverPath, std::string serverJarName, std::string serverAccount, int maxHeapAlloc,
		int minHeapAlloc, int gcThreadCount, std::string backupPath, std::vector<std::string> worldsToBackup, std::vector<std::string> javaArgs,
		std::vector<std::string> serverOptions) :
		serverName { serverName }, serverPath { serverPath }, serverJarName { serverJarName }, serverAccount { serverAccount }, maxHeapAlloc { maxHeapAlloc }, minHeapAlloc {
				minHeapAlloc }, gcThreadCount { gcThreadCount }, backupPath { backupPath }, worldsToBackup { worldsToBackup }, javaArgs { javaArgs }, serverOptions {
				serverOptions }, serverPropertiesParser((char*) "server.properties") {
	log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
	layout->setConversionPattern("%d %c [%p] %m%n ");

	log4cpp::Appender *serverFileAppender = new log4cpp::FileAppender(serverName, std::string(__LOG_DIR__) + "/servers/" + serverName + ".log");
	serverFileAppender->setLayout(layout);

	log = &log4cpp::Category::getInstance(std::string(serverName));
	log->addAppender(serverFileAppender);
	log->info(serverJarName);
	log->debug("VanillaServer::VanillaServer");
}
VanillaServer::~VanillaServer() {
	log->debug("VanillaServer::~VanillaServer");
}
/**
 * Updates the server to the specified version.
 * @param version
 */
void VanillaServer::updateServer(std::string version) {
	log->debug("VanillaServer::updateServer");
	stopServer();
	std::string url = "http://s3.amazonaws.com/Minecraft.Download/versions/" + version + "/minecraft_server." + version + ".jar";
	CURL *curl;
	FILE *fp;
	curl = curl_easy_init();
	if (curl) {
		unlink(serverJarName.c_str());
		fp = fopen(serverJarName.c_str(), "wb");
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		fclose(fp);
	}
	startServer();
}
/**
 * Backs up the server.
 */
void VanillaServer::backupServer() {
	log->debug("VanillaServer::backupServer");
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
 * Backs up the server to the specified path.
 * @param _backupPath
 */
void VanillaServer::backupServer(std::string _backupPath) {
	log->debug("VanillaServer::backupServer");
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
void VanillaServer::startServer() {
	log->debug("VanillaServer::startServer");
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
void VanillaServer::stopServer() {
	log->debug("VanillaServer::stopServer");
	if (isRunning()) {
		*this << "say SERVER SHUTTING DOWN IN 10 SECONDS." << std::endl;
		for (int i = 10; i > 0; --i) {
			*this << "say " + std::to_string(i) << std::endl;
			sleep(1);
		}
		*this << "say Saving map..." << std::endl;
		*this << "save-all" << std::endl;
		*this << "stop" << std::endl;
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
void VanillaServer::serverStatus() {
	log->debug("VanillaServer::serverStatus");
}
/**
 * Restarts the server.
 */
void VanillaServer::restartServer() {
	log->debug("VanillaServer::restartServer");
	if (isRunning()) {
		stopServer();
		startServer();
	}
}
/**
 * Sends the specified command to the server.
 * @param command
 */
void VanillaServer::sendCommand(std::string command) {
	log->debug("VanillaServer::sendCommand");
	if (isRunning()) {
		*this << command << std::endl;
	}
}
/**
 * Lists the players on the server.
 * @return
 */
std::string VanillaServer::listOnlinePlayers() {
	log->debug("VanillaServer::listOnlinePlayers");
	std::string* callbackOutput = new std::string;
	*callbackOutput = '\0';
	std::string output = '\0';
	Listener* listener = new Listener { callbackOutput, 10, false, 0, output };
	listeners->push_back(listener);
	*this << "list" << std::endl;
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
bool VanillaServer::listOnlinePlayers(__attribute__((unused))     std::string playerName) {
	log->debug("VanillaServer::listOnlinePlayers");
	std::string* callbackOutput = new std::string;
	*callbackOutput = '\0';
	std::string output = '\0';
	Listener* listener = new Listener { callbackOutput, 10, false, 0, output };
	listeners->push_back(listener);
	*this << "list" << std::endl;
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
