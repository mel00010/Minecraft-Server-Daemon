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
 
#ifndef BUKKITSERVER_H
#define BUKKITSERVER_H

#include <stddef.h>
#include <sstream>
#include <string>
#include <vector>

#include "Server.hpp"
#include "ServerPropertyFileParser.hpp"

namespace MinecraftServerDaemon {
class BukkitServer : public Server
{
	public:
		BukkitServer(
			std::string serverName,
			std::string serverPath, std::string serverJarName, std::string serverAccount,
			int maxHeapAlloc, int minHeapAlloc, int gcThreadCount,
			std::string backupPath,
			std::vector<std::string> worldsToBackup,
			std::vector<std::string> javaArgs, 
			std::vector<std::string> serverOptions
		);
		virtual ~BukkitServer();
		void updateServer(std::string version);
		void backupServer();
		void backupServer(std::string backupPath);
		void serverStatus();
		void startServer();
		void stopServer();
		void restartServer();
		void reloadServer();
		std::string listOnlinePlayers();
		bool listOnlinePlayers(std::string playerName);
		void sendCommand(std::string command);
		std::string getServerName()
		{
			return serverName;
		};
		ServerType getServerType()
		{
			return serverType;
		};
		ServerPropertyFileParser getServerPropertiesParser()
		{
			return serverPropertiesParser;
		};
	protected:
		ServerPropertyFileParser serverPropertiesParser;
		std::string serverName;
		ServerType serverType = BUKKIT;
		void logger(size_t linesRequested, std::stringstream* output, log4cpp::Category* log);
		void listOnlinePlayersCallback(size_t linesRequested, std::stringstream* output, log4cpp::Category* log);
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
#endif /* BUKKITSERVER_H */
