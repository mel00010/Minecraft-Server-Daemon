/*
 * genericServer.h
 * 
 * Copyright 2016 Mel McCalla <melmccalla@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
#ifndef GENERIC_SERVER_H
#define GENERIC_SERVER_H
#include "server.h"
#include "log4cpp/Category.hh"
#include <string>
#include <sstream>
#include <vector>
#include <thread>
#include <time.h>
namespace MinecraftServerService {
class GenericServer : public Server
{
	public:
		GenericServer(
			std::string serverName,
			std::string serverPath, std::string serverJarName, std::string serverAccount,
			int maxHeapAlloc, int minHeapAlloc, int gcThreadCount,
			std::string backupPath,
			std::vector<std::string> worldsToBackup,
			std::vector<std::string> javaArgs, 
			std::vector<std::string> serverOptions
		);
		virtual ~GenericServer();
		void updateServer();
		void backupServer();
		void backupServer(std::string backupPath);
		void serverStatus();
		void startServer();
		void stopServer();
		void restartServer();
		void reloadServer() {}
		std::string listOnlinePlayers() {}
		bool listOnlinePlayers(std::string playerName) {}
		void sendCommand(std::string command);
		std::string getServerName()
		{
			return serverName;
		};
	protected:
		std::string serverName;
		void logger(size_t linesRequested, std::stringstream* output, log4cpp::Category* log);
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
};
}
#endif /* GENERIC_SERVER_H */
