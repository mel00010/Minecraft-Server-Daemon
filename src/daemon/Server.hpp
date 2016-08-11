/*******************************************************************************
 *
 * Minecraft Server Daemon
 * Server.hpp
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

#ifndef DAEMON_SERVER_HPP_
#define DAEMON_SERVER_HPP_

#include <event2/event.h>
#include <event2/util.h>
#include <log4cpp/Category.hh>
#include <sys/wait.h>
#include <unistd.h>
#include <csignal>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include "ServerPropertyFileParser.hpp"

namespace MinecraftServerDaemon {
/**
 * Base class for all server classes.
 */
class Server {
	public:
		/**
		 * Stores values for use in the childProcessStdin and childProcessStdout arrays.
		 * PIPE_READ and PIPE_WRITE correspond to the ends of the pipes sed for reading and writing, respectively.
		 */
		enum PIPE_OPS {
			PIPE_READ = 0, PIPE_WRITE = 1,
		};
		/**
		 * Stores all of the possible types of a server.
		 * Used for identifying a server's type from a plain MinecraftServerDaemon::Server object.
		 */
		enum ServerType {
			GENERIC, VANILLA, BUKKIT, SPIGOT, FORGE, BUNGEECORD,
		};
		/**
		 * This struct is constructed to hold the data describing a function listening for output.
		 */
		struct Listener {
				std::string* callbackOutput;
				size_t lines;
				bool persistent;
				size_t currentLine;
				std::string output;
		};
		/**
		 * This struct contains the data passed to Server::serverOutputEvent by libevent.
		 */
		struct ServerOutputEventData {
				struct event_base* base;
				log4cpp::Category* log;
				std::vector<Listener*>* listeners;
		};
		/**
		 * Typedef defining the ServerManipulator type for use by the stream operators.
		 * @param
		 * @return
		 */
		typedef Server& (*ServerManipulator)(Server&);
		/**
		 * Typedef defining the CoutType type for use by the stream operators.
		 */
		typedef std::basic_ostream<char, std::char_traits<char> > CoutType;
		/**
		 * Typedef defining the StandardEndLine type for use by the stream operators.
		 * @param
		 * @return
		 */
		typedef CoutType& (*StandardEndLine)(CoutType&);

	public:
		virtual ~Server() {
		}
		/**
		 * Pure virtual function instantiated by derived server classes.
		 * @param version
		 */
		virtual void updateServer(std::string version) = 0;
		/**
		 * Pure virtual function instantiated by derived server classes.
		 */
		virtual void backupServer() = 0;
		/**
		 * Pure virtual function instantiated by derived server classes.
		 * @param backupPath
		 */
		virtual void backupServer(std::string backupPath) = 0;
		/**
		 * Pure virtual function instantiated by derived server classes.
		 */
		virtual void serverStatus() = 0;
		/**
		 * Pure virtual function instantiated by derived server classes.
		 */
		virtual void startServer() = 0;
		/**
		 * Pure virtual function instantiated by derived server classes.
		 */
		virtual void stopServer() = 0;
		/**
		 * Pure virtual function instantiated by derived server classes.
		 */
		virtual void restartServer() = 0;
		/**
		 * Pure virtual function instantiated by derived server classes.
		 */
		virtual void reloadServer() = 0;
		/**
		 * Pure virtual function instantiated by derived server classes.
		 * @return
		 */
		virtual std::string listOnlinePlayers() = 0;
		/**
		 * Pure virtual function instantiated by derived server classes.
		 * @param playerName
		 * @return
		 */
		virtual bool listOnlinePlayers(std::string playerName) = 0;
		/**
		 * Pure virtual function instantiated by derived server classes.
		 * @param command
		 */
		virtual void sendCommand(std::string command) = 0;
		/**
		 * Pure virtual function instantiated by derived server classes.
		 * @return
		 */
		virtual std::string getServerName() = 0;
		/**
		 * Pure virtual function instantiated by derived server classes.
		 * @return
		 */
		virtual ServerType getServerType() = 0;
		/**
		 * Pure virtual function instantiated by derived server classes.
		 * @return
		 */
		virtual ServerPropertyFileParser getServerPropertiesParser() = 0;
	protected:
		/**
		 * Holds the server process's process id.
		 */
		int serverPID = -1;
		/**
		 * Holds the server process's user id.
		 */
		int childProcessUID;
		/**
		 * Holds the server process's group id.
		 */
		int childProcessGID;
		/**
		 * Holds the server process's stdin pipe.
		 */
		int childProcessStdin[2];
		/**
		 * Holds the server process's stdout pipe.
		 */
		int childProcessStdout[2];
		/**
		 * Log4cpp object used for logging.
		 */
		log4cpp::Category* log;
		/**
		 * Libevent base object.
		 */
		struct event_base* base = event_base_new();
		/**
		 * Vector of listener objects used by Server::serverOutputEvent to distribute data.
		 */
		std::vector<Listener*>* listeners = new std::vector<Listener*>;
	public:
		/**
		 * Tests if the server process is running.
		 * @return
		 */
		inline bool isRunning() {
			if (serverPID != -1) {
				while (waitpid(-1, 0, WNOHANG) > 0) {
					// Wait for defunct....
				}
				if (0 == kill(serverPID, 0)) {
					return 1; // Process exists
				} else {
					return 0;
				}
			} else {
				return 0;
			}
		}
		template<typename T>
		/**
		 * Stream insertion operator.
		 * @param x
		 * @return
		 */
		inline Server& operator<<(const T& x) {
			write(childProcessStdin[PIPE_WRITE], x, sizeof(x));
			return *this;
		}
		/**
		 * Stream insertion operator handling std::string.
		 * @param x
		 * @return
		 */
		inline Server& operator<<(__attribute__((unused))     const std::string& x) {
			write(childProcessStdin[PIPE_WRITE], x.c_str(), x.size());
			return *this;
		}
		/**
		 * Stream insertion operator.
		 * @param in
		 * @return
		 */
		inline std::istream& operator>>(__attribute__((unused))     std::istream &in) {
			std::string s(std::istreambuf_iterator<char>(in), { });
			write(childProcessStdin[PIPE_WRITE], s.c_str(), s.size());
			return in;
		}
		/**
		 * Stream insertion operator handling std::endl.
		 * @param manip
		 * @return
		 */
		inline Server& operator<<(__attribute__((unused))     StandardEndLine manip) {
			// Call the function, but we cannot return it's value
			write(childProcessStdin[PIPE_WRITE], "\n", sizeof("\n"));
			return *this;
		}
	protected:
		/**
		 * Function launched as a thread to listen for output on the server process.
		 * Uses libevent to call Server::serverOutputEvent when there is data on the server process's output pipe.
		 * @param serverPID
		 * @param childProcessStdout
		 * @param base
		 * @param log
		 * @param listeners
		 */
		static void outputListenerThread(int serverPID, int childProcessStdout, struct event_base* base, log4cpp::Category* log,
				std::vector<Listener*>* listeners);
		/**
		 * Gets data from child process's output pipe, splits it into lines, and distributes it to the functions waiting for server output.
		 * @param fd
		 * @param event
		 * @param arg
		 */
		static void serverOutputEvent(evutil_socket_t fd, short event, void *arg);
		/**
		 * Takes a username and gets its user id and group id from it.
		 * It stores this information in the member variables childProcessUID and childProcessGID.
		 * @param user
		 */
		void getUIDAndGIDFromUsername(const char* user);
		/**
		 * Launches the server process and redirects its stdin, stdout, and stderr into pipes.
		 * It stores the file descriptors for these pipes in childProcessStdin[PIPE_WRITE] and  childProcessStdout[PIPE_READ].
		 * (Stdout and stderr share a pipe.)
		 * @param serverPath
		 * @param serverJarName
		 * @param serverAccount
		 * @param maxHeapAlloc
		 * @param minHeapAlloc
		 * @param gcThreadCount
		 * @param javaArgs
		 * @param serverOptions
		 */
		void launchServerProcess(std::string serverPath, std::string serverJarName, std::string serverAccount, int maxHeapAlloc, int minHeapAlloc,
				int gcThreadCount, std::vector<std::string> javaArgs, std::vector<std::string> serverOptions);
};

} /* namespace MinecraftServerDaemon */
#endif /* DAEMON_SERVER_HPP_ */
