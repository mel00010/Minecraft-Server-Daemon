/*******************************************************************************
 *
 * Minecraft Server Daemon
 * BungeeCordServer.hpp
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

#ifndef DAEMON_BUNGEECORDSERVER_HPP_
#define DAEMON_BUNGEECORDSERVER_HPP_

#include <log4cpp/Category.hh>
#include <stddef.h>
#include <sstream>
#include <string>
#include <vector>

#include "Server.hpp"
#include "ServerPropertyFileParser.hpp"

namespace MinecraftServerDaemon {
/**
 * Class handling BungeeCord minecraft servers.
 */
class BungeeCordServer: public Server {
	public:
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
		BungeeCordServer(std::string serverName, std::string serverPath, std::string serverJarName, std::string serverAccount, int maxHeapAlloc,
				int minHeapAlloc, int gcThreadCount, std::string backupPath, std::vector<std::string> javaArgs, std::vector<std::string> serverOptions);
		/**
		 * Destructor.
		 */
		virtual ~BungeeCordServer();
		/**
		 * Currently unimplemented.
		 * @param version
		 */
		void updateServer(std::string version);
		/**
		 * Backs up the server.
		 */
		void backupServer();
		/**
		 * Backs up the server to a specified path.
		 * @param _backupPath
		 */
		void backupServer(std::string backupPath);
		/**
		 * Currently unimplemented.
		 */
		void serverStatus();
		/**
		 * Starts the server.
		 */
		void startServer();
		/**
		 * Stops the server.
		 */
		void stopServer();
		/**
		 * Restarts the server.
		 */
		void restartServer();
		/**
		 * Reloads the server.
		 */
		void reloadServer();
		/**
		 * Lists the players on the server.
		 * @return
		 */
		std::string listOnlinePlayers();
		/**
		 * Tests if the specified player is on the server.  (Currently non functional)
		 * @param playerName
		 * @return
		 */
		bool listOnlinePlayers(std::string playerName);
		/**
		 * Lists the players on the server.
		 * @return
		 */
		void sendCommand(std::string command);
		/**
		 * Returns the server name.
		 * @return
		 */
		std::string getServerName() {
			return serverName;
		}
		/**
		 * Returns the server type.
		 * @return
		 */
		ServerType getServerType() {
			return serverType;
		}
		/**
		 * Currently unimplemented.
		 * @return
		 */
		ServerPropertyFileParser getServerPropertiesParser() {
		}
	protected:
		/**
		 * Holds the server name.
		 */
		std::string serverName;
		/**
		 * Holds the server type.
		 */
		ServerType serverType = BUNGEECORD;
		/**
		 * Holds the path of the server directory.
		 */
		std::string serverPath;
		/**
		 * Holds the server jar name.
		 */
		std::string serverJarName;
		/**
		 * Holds the name of the user the server process is to be run as.
		 */
		std::string serverAccount;
		/**
		 * Holds the maximum value for heap memory the child java process can have.
		 */
		int maxHeapAlloc;
		/**
		 * Holds the minimum value for heap memory the child java process can have.
		 */
		int minHeapAlloc;
		/**
		 * Holds the number of garbage collection threads the child java process will have.
		 */
		int gcThreadCount;
		/**
		 * Holds the path that backups will be saved to.  (Relative to the path held in serverPath)
		 */
		std::string backupPath;
		/**
		 * Holds the names of the worlds that will be backed up.
		 */
		std::vector<std::string> worldsToBackup;
		/**
		 * Holds arguments to pass to the child java process.
		 */
		std::vector<std::string> javaArgs;
		/**
		 * Holds command line options for the server.
		 */
		std::vector<std::string> serverOptions;
};

} /* namespace MinecraftServerDaemon */
#endif /* DAEMON_BUNGEECORDSERVER_HPP_ */
