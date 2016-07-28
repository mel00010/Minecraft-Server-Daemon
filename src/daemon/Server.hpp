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

class Server {
	public:
		enum PIPE_OPS {
			PIPE_READ = 0, PIPE_WRITE = 1,
		};
		enum ServerType {
			GENERIC, VANILLA, BUKKIT, SPIGOT, FORGE, BUNGEECORD,
		};
		struct Listener {
				std::string* callbackOutput;
				size_t lines;
				bool persistent;
				size_t currentLine;
				std::string output;
		};
		struct ServerOutputEventData {
				struct event_base* base;
				log4cpp::Category* log;
				std::vector<Listener*>* listeners;
		};
		typedef Server& (*ServerManipulator)(Server&);
		typedef std::basic_ostream<char, std::char_traits<char> > CoutType;
		typedef CoutType& (*StandardEndLine)(CoutType&);

	public:
		virtual ~Server() {
		}
		virtual void updateServer(std::string version) = 0;
		virtual void backupServer() = 0;
		virtual void backupServer(std::string backupPath) = 0;
		virtual void serverStatus() = 0;
		virtual void startServer() = 0;
		virtual void stopServer() = 0;
		virtual void restartServer() = 0;
		virtual void reloadServer() = 0;
		virtual std::string listOnlinePlayers() = 0;
		virtual bool listOnlinePlayers(std::string playerName) = 0;
		virtual void sendCommand(std::string command) = 0;
		virtual std::string getServerName() = 0;
		virtual ServerType getServerType() = 0;
		virtual ServerPropertyFileParser getServerPropertiesParser() = 0;
	protected:
		int serverPID = -1;
		int childProcessUID;
		int childProcessGID;
		int childProcessStdin[2];
		int childProcessStdout[2];
		log4cpp::Category* log;
		struct event_base* base = event_base_new();
		std::vector<Listener*>* listeners = new std::vector<Listener*>;
	public:
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
		inline Server& operator<<(const T& x) {
			write(childProcessStdin[PIPE_WRITE], x, sizeof(x));
			return *this;
		}
		inline Server& operator<<(__attribute__((unused))  const std::string& x) {
			write(childProcessStdin[PIPE_WRITE], x.c_str(), x.size());
			return *this;
		}
		inline std::istream& operator>>(__attribute__((unused))  std::istream &in) {
			std::string s(std::istreambuf_iterator<char>(in), { });
			write(childProcessStdin[PIPE_WRITE], s.c_str(), s.size());
			return in;
		}
		// define an operator<< to take in std::endl
		inline Server& operator<<(__attribute__((unused))  StandardEndLine manip) {
			// call the function, but we cannot return it's value
			write(childProcessStdin[PIPE_WRITE], "\n", sizeof("\n"));
			return *this;
		}
	protected:
		static void outputListenerThread(int serverPID, int childProcessStdout, struct event_base* base, log4cpp::Category* log,
				std::vector<Listener*>* listeners);
		static void serverOutputEvent(evutil_socket_t fd, short event, void *arg);
		void getUIDAndGIDFromUsername(const char* user);
		void launchServerProcess(std::string serverPath, std::string serverJarName, std::string serverAccount, int maxHeapAlloc, int minHeapAlloc,
				int gcThreadCount, std::vector<std::string> javaArgs, std::vector<std::string> serverOptions);
};

} /* namespace MinecraftServerDaemon */
#endif /* DAEMON_SERVER_HPP_ */
