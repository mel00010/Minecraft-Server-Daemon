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

#include <Daemon/MainLoop.hpp>
#include <event2/event.h>
#include <IPC/InputMessage.hpp>
#include <IPC/IPC.hpp>
#include <IPC/OutputMessage.hpp>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>
#include <csignal>
#include <cstdlib>
#include <iterator>
#include <string>
#include <Python.h>
#include <PythonExtension/ScriptHandler.hpp>
#include <Server/Player.hpp>
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
	cb_data* data = new cb_data { servers, root, base, nullptr };
	event = event_new(base, controlSocket, EV_READ | EV_PERSIST | EV_ET, acceptConnection, data);
	data->event = event;
	event_add(event, NULL);
	event_base_dispatch(base);
}
void readAndProcessMessage(int controlSocket, __attribute__((unused)) short what, void *arg) {
	std::vector<MinecraftServerDaemon::Server*>* servers = ((cb_data *) arg)->servers;
	log4cpp::Category& root = ((cb_data *) arg)->root;
	MinecraftServerDaemon::InputMessage message = readFromSocket(controlSocket, root);
	MinecraftServerDaemon::OutputMessage output;
	root.debug("In readAndProcessMessage");
	if (message.error) {
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			(*i)->unRegisterOutputListener(controlSocket);
		}
		event_free(((cb_data *) arg)->event);
		return;
	} else if (message.command == "startServer") {
		root.debug("Command received:  startServer");
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			root.debug((*i)->getServerName());
			if ((*i)->getServerName() == message.server) {
				(*i)->startServer();
				output.success = true;
				break;
			}
		}
	} else if (message.command == "startAll") {
		root.debug("Command received:  startAll");
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			(*i)->startServer();
		}
		output.success = true;
	} else if (message.command == "stopServer") {
		root.debug("Command received:  stopServer");
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			root.debug((*i)->getServerName());
			if ((*i)->getServerName() == message.server) {
				root.debug("Found server");
				(*i)->stopServer();
				output.success = true;
				break;
			}
		}
	} else if (message.command == "stopAll") {
		root.debug("Command received:  stopAll");
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			(*i)->stopServer();
		}
		output.success = true;
	} else if (message.command == "restartServer") {
		root.debug("Command received:  restartServer");
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			root.debug((*i)->getServerName());
			if ((*i)->getServerName() == message.server) {
				(*i)->restartServer();
				output.success = true;
				break;
			}
		}
	} else if (message.command == "restartAll") {
		root.debug("Command received:  restartAll");
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			(*i)->restartServer();
		}
		output.success = true;
	} else if (message.command == "serverStatus") {
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			root.debug((*i)->getServerName());
			if ((*i)->getServerName() == message.server) {
				root.warn("Status not implemented yet");
				break;
			}
		}
	} else if (message.command == "registerOutputListener") {
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			root.debug((*i)->getServerName());
			if ((*i)->getServerName() == message.server) {
				if ((*i)->registerOutputListener(controlSocket)) {
					output.success = true;
				} else {
					output.success = false;
					output.failureReason = "";
				}
				break;
			}
		}
	} else if (message.command == "unRegisterOutputListener") {
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			root.debug((*i)->getServerName());
			if ((*i)->getServerName() == message.server) {
				if ((*i)->unRegisterOutputListener(controlSocket)) {
					output.success = true;
				} else {
					output.success = false;
				output.failureReason = "";
				}
				break;
			}
		}
	} else if (message.command == "listServers") {
		root.debug("Command received:  listServers");
		std::string serverList;
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			root.debug((*i)->getServerName());
			if (i != servers->begin()) {
			serverList = serverList + "\n" + (*i)->getServerName();
			} else {
				serverList = serverList + (*i)->getServerName();
			}
		}
		output.messageData = serverList;
	} else if (message.command == "sendCommand") {
			for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			root.debug((*i)->getServerName());
			if ((*i)->getServerName() == message.server) {
			root.debug("Found server");
				(*i)->sendCommand(message.serverCommand);
				output.success = true;
				break;
			}
		}
	} else if (message.command == "listOnlinePlayers") {
		root.debug("Command received:  listOnlinePlayers");
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			root.debug((*i)->getServerName());
			if ((*i)->getServerName() == message.server) {
				(*i)->listOnlinePlayers();
				break;
			}
		}
	} else if (message.command == "listOnlinePlayersFiltered") {
		root.debug("Command received:  listOnlinePlayersFiltered");
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			root.debug((*i)->getServerName());
			if ((*i)->getServerName() == message.server) {
				(*i)->listOnlinePlayers(message.player);
				break;
			}
		}
	} else if (message.command == "updateServer") {
		root.debug("Command received:  updateServer");
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			root.debug((*i)->getServerName());
			if ((*i)->getServerName() == message.server) {
				if ((*i)->getServerType() == MinecraftServerDaemon::Server::ServerType::VANILLA) {
					(*i)->updateServer(message.version);
					output.success = true;
					break;
				}
			}
		}
	} else if (message.command == "updateAll") {
		root.debug("Command received:  updateAll");
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			if ((*i)->getServerType() == MinecraftServerDaemon::Server::ServerType::VANILLA) {
				(*i)->updateServer(message.version);
				output.success = true;
				break;
			}
		}
		output.success = true;
	} else if (message.command == "backupServer") {
		root.debug("Command received:  backupServer");
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			if ((*i)->getServerName() == message.server) {
				(*i)->backupServer();
				output.success = true;
				break;
			}
		}
	} else if (message.command == "backupAll") {
		root.debug("Command received:  backupAll");
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			(*i)->backupServer();
		}
		output.success = true;
	} else if (message.command == "stopDaemon") {
		root.debug("Command received:  stopDaemon");
		std::string serverList;
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			root.debug((*i)->getServerName());
			(*i)->stopServer();
		}
		Py_Finalize();
		root.notice("All servers stopped");
		root.notice("Stopping");
		output.messageData = "Stopped";
		exit(0);
	} else if (message.command == "opPlayer") {
		root.debug("Command received:  opPlayer");
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			if ((*i)->getServerName() == message.server) {
				for (std::vector<MinecraftServerDaemon::Player*>::iterator j = (*i)->getPlayerVector()->begin(); j != (*i)->getPlayerVector()->end(); j++) {
					if ((*j)->getPlayerName() == message.player) {
						(*j)->op();
						output.success = true;
						break;
					}
				}
				break;
			}
		}
	} else if (message.command == "deopPlayer") {
		root.debug("Command received:  deopPlayer");
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			if ((*i)->getServerName() == message.server) {
				for (std::vector<MinecraftServerDaemon::Player*>::iterator j = (*i)->getPlayerVector()->begin(); j != (*i)->getPlayerVector()->end(); j++) {
					if ((*j)->getPlayerName() == message.player) {
						(*j)->deop();
						output.success = true;
						break;
					}
				}
				break;
			}
		}
	} else if (message.command == "kickPlayer") {
		root.debug("Command received:  kickPlayer");
		bool kickedPlayer = false;
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			if ((*i)->getServerName() == message.server) {
				for (std::vector<MinecraftServerDaemon::Player*>::iterator j = (*i)->getPlayerVector()->begin(); j != (*i)->getPlayerVector()->end(); j++) {
					if ((*j)->getPlayerName() == message.player) {
						root.debug((*j)->getPlayerName());
						(*j)->kick();
						kickedPlayer = true;
						break;
					}
				}
				if (kickedPlayer) {
					output.success = true;
				} else {
					output.success = false;
					output.failureReason = "";
				}
				break;
			}

		}
	} else if (message.command == "kickPlayerReason") {
		root.debug("Command received:  kickPlayerReason");
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			if ((*i)->getServerName() == message.server) {
				for (std::vector<MinecraftServerDaemon::Player*>::iterator j = (*i)->getPlayerVector()->begin(); j != (*i)->getPlayerVector()->end(); j++) {
					if ((*j)->getPlayerName() == message.player) {
						(*j)->kick(message.reason);
						output.success = true;
						break;
					}
				}
				break;
			}
		}
	} else if (message.command == "banPlayer") {
		root.debug("Command received:  banPlayer");
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			if ((*i)->getServerName() == message.server) {
				for (std::vector<MinecraftServerDaemon::Player*>::iterator j = (*i)->getPlayerVector()->begin(); j != (*i)->getPlayerVector()->end(); j++) {
					if ((*j)->getPlayerName() == message.player) {
						(*j)->ban();
						output.success = true;
						break;
					}
				}
				break;
			}
		}
	} else if (message.command == "banPlayerReason") {
		root.debug("Command received:  banPlayerReason");
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			if ((*i)->getServerName() == message.server) {
				for (std::vector<MinecraftServerDaemon::Player*>::iterator j = (*i)->getPlayerVector()->begin(); j != (*i)->getPlayerVector()->end(); j++) {
					if ((*j)->getPlayerName() == message.player) {
						(*j)->ban(message.reason);
						output.success = true;
						break;
					}
				}
				break;
			}
		}
	} else if (message.command == "pardonPlayer") {
		root.debug("Command received:  pardonPlayer");
		for (std::vector<MinecraftServerDaemon::Server*>::iterator i = servers->begin(); i != servers->end(); i++) {
			if ((*i)->getServerName() == message.server) {
				(*((*i)->getPlayerVector()->begin()))->pardon(message.player);
				output.success = true;
				break;
			}
		}
	} else if (message.command == "runScript") {
		root.debug("Command received:  runScript");
		scriptLauncher(message.script, servers, &root);
		output.success = true;
	}
	output.mode = "output";
	output.command = message.command;
	root.debug("Writing message to socket");

	writeToSocket(output, controlSocket, root);
	event_add(((cb_data *) arg)->event, NULL);
}
/**
 * This function handles the processing of commands recieved from the control socket.  Called by libevent when the control socket is ready for reading.
 * @param _controlSocket
 * @param what
 * @param arg
 */
void acceptConnection(int _controlSocket, __attribute__((unused)) short what, void *arg) {
	struct sockaddr_un peer_addr;
	socklen_t peer_addr_size;
	peer_addr_size = sizeof(struct sockaddr_un);
	int controlSocket = accept(_controlSocket, (struct sockaddr *) &peer_addr, &peer_addr_size);
	struct event *event;
	cb_data* data = new cb_data { ((cb_data *) arg)->servers, ((cb_data *) arg)->root, ((cb_data *) arg)->base, nullptr };
	event = event_new(((cb_data *) arg)->base, controlSocket, EV_READ | EV_PERSIST, readAndProcessMessage, data);
	data->event = event;
	event_add(event, NULL);
}
