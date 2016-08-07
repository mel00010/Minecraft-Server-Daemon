/*******************************************************************************
 *
 * Minecraft Server Daemon
 * MainLoop.cpp
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

#include <event2/event.h>
#include <log4cpp/Category.hh>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>
#include "MainLoop.hpp"
#include <cstdlib>
#include <iomanip>
#include <iterator>
#include <sstream>
/**
 * Catches the SIGINT signal and kills all of the child processes, then sends the SIGINT signal again to kill the program.
 * @param sig
 */
void sigint_handler(__attribute__((unused)) int sig) {
	wait(NULL);
	signal(SIGINT, SIG_DFL);
	kill(getpid(), SIGINT);
}

/**
 * Initializes the main control loop with libevent.
 * It creates an event that triggers whenever the control socket is ready for reading.  This event calls recieveCommend to do the processing.
 * @param servers
 * @param root
 * @param controlSocket
 * @param base
 */
void mainLoop(std::vector<MinecraftServerDaemon::Server*>* servers, log4cpp::Category& root, int controlSocket, struct event_base* base) {
	root.info("Starting main loop");
	struct event *event;
	cb_data* data = new cb_data { servers, root };
	event = event_new(base, controlSocket, EV_READ | EV_PERSIST | EV_ET, recieveCommand, data);
	event_add(event, NULL);
	event_base_dispatch(base);
}
/**
 * Writes the specified message to the control socket.
 * @param message
 * @param controlSocket
 * @param root
 */
void writeToSocket(std::string message, int controlSocket, __attribute__((unused))       log4cpp::Category& root) {
	std::ostringstream ss;
	ss << std::setw(4) << std::setfill('0') << message.size();
	std::string buf = ss.str() + message;
	send(controlSocket, buf.c_str(), buf.size(), 0);
}
/**
 * Reads from the control socket.
 * @param controlSocket
 * @param root
 * @return
 */
std::string readFromSocket(int controlSocket, log4cpp::Category& root) {
	int rc;
	char buff[4];
	rc = recv(controlSocket, buff, 4, 0);
	if (rc == -1) {
		root.error("Read from control socket failed");
		return std::string("error");
	} else if (rc == 0) {
		root.info("Client disconnected from control socket");
		return std::string("error");
	} else {
		int size = atoi(buff);
		char line[size];
		rc = recv(controlSocket, line, size, 0);
		line[rc] = '\0';
		return std::string(line);
	}
}
/**
 * This function handles the processing of commands recieved from the control socket.  Called by libevent when the control socket is ready for reading.
 * @param _controlSocket
 * @param what
 * @param arg
 */
void recieveCommand(int _controlSocket, __attribute__((unused)) short what, void *arg) {
	struct sockaddr_un peer_addr;
	socklen_t peer_addr_size;
	peer_addr_size = sizeof(struct sockaddr_un);
	int controlSocket = accept(_controlSocket, (struct sockaddr *) &peer_addr, &peer_addr_size);
	//~ struct cb_data *data = (cb_data *)arg;
	std::vector<MinecraftServerDaemon::Server*>* servers = ((cb_data *) arg)->servers;
	log4cpp::Category& root = ((cb_data *) arg)->root;
	std::string command(readFromSocket(controlSocket, root));
	if (command == "error") {
		return;
	} else if (command == "startServer") {
		root.debug("Command received:  startServer");
		std::string serverRequested(readFromSocket(controlSocket, root));
		root.debug("Server name received");
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			root.debug((*i)->getServerName());
			if ((*i)->getServerName() == serverRequested) {
				(*i)->startServer();
				writeToSocket("success", controlSocket, root);
				break;
			}
		}
	} else if (command == "startAll") {
		root.debug("Command received:  startAll");
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			(*i)->startServer();
		}
		writeToSocket("success", controlSocket, root);
	} else if (command == "stopServer") {
		root.debug("Command received:  stopServer");
		std::string serverRequested(readFromSocket(controlSocket, root));
		root.debug("Server name received");
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			root.debug((*i)->getServerName());
			if ((*i)->getServerName() == serverRequested) {
				root.debug("Found server");
				(*i)->stopServer();
				writeToSocket("success", controlSocket, root);
				break;
			}
		}
	} else if (command == "stopAll") {
		root.debug("Command received:  stopAll");
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			(*i)->stopServer();
		}
		writeToSocket("success", controlSocket, root);
	} else if (command == "restartServer") {
		root.debug("Command received:  restartServer");
		std::string serverRequested(readFromSocket(controlSocket, root));
		root.debug("Server name received");
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			root.debug((*i)->getServerName());
			if ((*i)->getServerName() == serverRequested) {
				(*i)->restartServer();
				writeToSocket("success", controlSocket, root);
				break;
			}
		}
	} else if (command == "restartAll") {
		root.debug("Command received:  restartAll");
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			(*i)->restartServer();
		}
		writeToSocket("success", controlSocket, root);
	} else if (command == "serverStatus") {
		std::string serverRequested(readFromSocket(controlSocket, root));
		root.debug("Server name received");
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			root.debug((*i)->getServerName());
			if ((*i)->getServerName() == serverRequested) {
				root.warn("Status not implemented yet");
				break;
			}
		}
	} else if (command == "listServers") {
		root.debug("Command received:  listServers");
		std::string serverList;
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			root.debug((*i)->getServerName());
			serverList = serverList + (*i)->getServerName() + "\n";
		}
		writeToSocket(serverList, controlSocket, root);
	} else if (command == "sendCommand") {
		std::string serverRequested(readFromSocket(controlSocket, root));
		root.debug(serverRequested);
		root.debug("Server name received");
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			root.debug((*i)->getServerName());
			if ((*i)->getServerName() == serverRequested) {
				root.debug("Found server");
				std::string serverCommand(readFromSocket(controlSocket, root));
				root.debug("Server command received");
				root.debug(serverCommand);
				(*i)->sendCommand(serverCommand);
				writeToSocket("success", controlSocket, root);
				break;
			}
		}
	} else if (command == "listOnlinePlayers") {
		root.debug("Command received:  listOnlinePlayers");
		std::string serverRequested(readFromSocket(controlSocket, root));
		root.debug("Server name received");
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			root.debug((*i)->getServerName());
			if ((*i)->getServerName() == serverRequested) {
				(*i)->listOnlinePlayers();
				break;
			}
		}
	} else if (command == "listOnlinePlayersFiltered") {
		root.debug("Command received:  listOnlinePlayersFiltered");
		std::string serverRequested(readFromSocket(controlSocket, root));
		root.debug("Server name received");
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			root.debug((*i)->getServerName());
			if ((*i)->getServerName() == serverRequested) {
				std::string playerName(readFromSocket(controlSocket, root));
				root.debug("playerName received");
				root.debug(playerName);
				(*i)->listOnlinePlayers(playerName);
				break;
			}
		}
	} else if (command == "updateServer") {
		root.debug("Command received:  updateServer");
		std::string serverRequested(readFromSocket(controlSocket, root));
		root.debug("Server name received");
		std::string versionRequested(readFromSocket(controlSocket, root));
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			root.debug((*i)->getServerName());
			if ((*i)->getServerName() == serverRequested) {
				if ((*i)->getServerType() == MinecraftServerDaemon::Server::ServerType::VANILLA) {
					(*i)->updateServer(versionRequested);
					writeToSocket("success", controlSocket, root);
					break;
				}
			}
		}
	} else if (command == "updateAll") {
		root.debug("Command received:  updateAll");
		std::string versionRequested(readFromSocket(controlSocket, root));
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			if ((*i)->getServerType() == MinecraftServerDaemon::Server::ServerType::VANILLA) {
				(*i)->updateServer(versionRequested);
				writeToSocket("success", controlSocket, root);
				break;
			}
		}
		writeToSocket("success", controlSocket, root);
	} else if (command == "backupServer") {
		root.debug("Command received:  backupServer");
		std::string serverRequested(readFromSocket(controlSocket, root));
		root.debug("Server name received");
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			if ((*i)->getServerName() == serverRequested) {
				(*i)->backupServer();
				writeToSocket("success", controlSocket, root);
				break;
			}
		}
	} else if (command == "backupAll") {
		root.debug("Command received:  backupAll");
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			(*i)->backupServer();
		}
		writeToSocket("success", controlSocket, root);
	} else if (command == "stopDaemon") {
		root.debug("Command received:  stopDaemon");
		std::string serverList;
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			root.debug((*i)->getServerName());
			(*i)->stopServer();
		}
		root.notice("All servers stopped");
		root.notice("Stopping");
		writeToSocket("Stopped", controlSocket, root);
		exit(0);
	}
}
