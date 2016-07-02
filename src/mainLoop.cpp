/*
 * mainLoop.cpp
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
#include "server.h"
#include "parser.h"
#include "mainLoop.h"
#include "setupServers.h"
#include <json/json.h>
#include "log4cpp/Category.hh"
#include <string>
#include <iomanip>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#include <sys/un.h>
#include <event2/event.h>
void sigint_handler(int sig)
{
	wait(NULL);
	signal(SIGINT, SIG_DFL);
	kill(getpid(), SIGINT);
}
struct cb_data
{
	std::vector<MinecraftServerService::Server*>* servers;
	log4cpp::Category& root;
};
void mainLoop (std::vector<MinecraftServerService::Server*>* servers, log4cpp::Category& root, int controlSocket, struct event_base* base) 
{
	root.info("Starting main loop");
	struct event *event;
	cb_data* data= new cb_data{servers, root};
	event = event_new(base, controlSocket, EV_READ|EV_PERSIST|EV_ET, recieveCommand, data);
	event_add(event, NULL);
	event_base_dispatch(base);
}
void writeToSocket(std::string message, int controlSocket, log4cpp::Category& root)
{
	std::ostringstream ss;
	ss << std::setw(4) << std::setfill('0') << message.size();
	std::string buf = ss.str() + message;
	send(controlSocket, buf.c_str(), buf.size(), 0);
}
std::string readFromSocket(int controlSocket, log4cpp::Category& root)
{
	int rc;
	char buff[4];
	rc=recv(controlSocket,buff,4,0);
	int size = atoi(buff);
	char line[size];
	rc = recv(controlSocket,line,size,0);
	line[rc] = '\0';
	return std::string(line);
}
void recieveCommand(int _controlSocket, short what, void *arg)
{
	struct sockaddr_un peer_addr;
    socklen_t peer_addr_size;
    peer_addr_size = sizeof(struct sockaddr_un);
	int controlSocket = accept(_controlSocket, (struct sockaddr *) &peer_addr,
                 &peer_addr_size);
	//~ struct cb_data *data = (cb_data *)arg;
	std::vector<MinecraftServerService::Server*>* servers = ((cb_data *)arg)->servers;
	log4cpp::Category& root = ((cb_data *)arg)->root;
	int rc;
	char buf[4];
	rc=recv(controlSocket,buf,4,0);
	if (rc == -1) {
	  root.error("Read from control socket failed");
	}
	else if (rc == 0) {
		root.info("Client disconnected from control socket");
	} else {
		buf[rc] = '\0';
		int size = atoi(buf);
		root.debug(std::to_string(size));
		char line[size];
		rc = recv(controlSocket,line,size,0);
		line[rc] = '\0';
		root.debug("Client connected to control pipe");
		std::string command(line);
		root.debug(command);
		if (command == "startServer") {
			root.debug("Command recieved:  startServer");
			rc=recv(controlSocket,buf,4,0);
			size = atoi(buf);
			root.debug(std::to_string(size));
			char line[size];
			rc = recv(controlSocket,line,size,0);
			line[rc] = '\0';
			std::string serverRequested(line);
			root.debug("Server name recieved");
			for( std::vector<MinecraftServerService::Server*>::iterator i = servers->begin() ; i != servers->end(); i++) {
				root.debug((*i)->getServerName());
				if ((*i)->getServerName() == serverRequested) {
					(*i)->startServer();
					//~ root.debug("Server starting not implemented yet");
					writeToSocket("success", controlSocket, root);
					break;
				}
			}
		} else if (command == "startAll") {
			root.debug("Command recieved:  startAll");
			for( std::vector<MinecraftServerService::Server*>::iterator i = servers->begin() ; i != servers->end(); i++) {
				(*i)->startServer();
			}
			writeToSocket("success", controlSocket, root);
		} else if (command == "stopServer") {
			root.debug("Command recieved:  stopServer");
			rc=recv(controlSocket,buf,4,0);
			size = atoi(buf);
			root.debug(std::to_string(size));
			char line[size];
			rc = recv(controlSocket,line,size,0);
			line[rc] = '\0';
			std::string serverRequested(line);
			root.debug("Server name recieved");
			for( std::vector<MinecraftServerService::Server*>::iterator i = servers->begin() ; i != servers->end(); i++) {
				root.debug((*i)->getServerName());
				if ((*i)->getServerName() == serverRequested) {
					root.debug("Found server");
					//~ if (!(*i)->getStream()->rdbuf()->exited())
					//~ {
						(*i)->stopServer();
						//~ if ((*i)->getStream()->rdbuf()->exited())
						//~ {
							writeToSocket("success", controlSocket, root);
						//~ } else {
							//~ writeToSocket("failure", controlSocket, root);
						//~ }
						break;
					//~ } else {
						//~ writeToSocket((*i)->getServerName()+"not running", controlSocket, root);
					//~ }
				}
			}
		} else if (command == "stopAll") {
			root.debug("Command recieved:  stopAll");
			for( std::vector<MinecraftServerService::Server*>::iterator i = servers->begin() ; i != servers->end(); i++) {
				(*i)->stopServer();
			}
			writeToSocket("success", controlSocket, root);
		} else if (command == "restartServer") {
			root.debug("Command recieved:  restartServer");
			rc=recv(controlSocket,buf,4,0);
			size = atoi(buf);
			root.debug(std::to_string(size));
			char line[size];
			rc = recv(controlSocket,line,size,0);
			line[rc] = '\0';
			std::string serverRequested(line);
			root.debug("Server name recieved");
			for( std::vector<MinecraftServerService::Server*>::iterator i = servers->begin() ; i != servers->end(); i++) {
				root.debug((*i)->getServerName());
				if ((*i)->getServerName() == serverRequested) {
					(*i)->restartServer();
					writeToSocket("success", controlSocket, root);
					break;
				}
			}
		} else if (command == "restartAll") {
			root.debug("Command recieved:  restartAll");
			for( std::vector<MinecraftServerService::Server*>::iterator i = servers->begin() ; i != servers->end(); i++) {
				(*i)->restartServer();
			}
			writeToSocket("success", controlSocket, root);
		} else if (command == "serverStatus") {
			root.debug("Command recieved:  serverStatus");
			size = atoi(buf);
			root.debug(std::to_string(size));
			char line[size];
			rc = recv(controlSocket,line,size,0);
			line[rc] = '\0';
			std::string serverRequested(line);
			root.debug("Server name recieved");
			for( std::vector<MinecraftServerService::Server*>::iterator i = servers->begin() ; i != servers->end(); i++) {
				root.debug((*i)->getServerName());
				if ((*i)->getServerName() == serverRequested) {
					root.warn("Status not implemented yet");
					break;
				}
			}
		} else if (command == "listServers") {
			root.debug("Command recieved:  listServers");
			std::string serverList;
			for( std::vector<MinecraftServerService::Server*>::iterator i = servers->begin() ; i != servers->end(); i++) {
				root.debug((*i)->getServerName());
				serverList = serverList+(*i)->getServerName() + "\n";
			}
			writeToSocket(serverList, controlSocket, root);
		} else if (command == "sendCommand") {
			root.debug("Command recieved:  sendCommand");
			rc=recv(controlSocket,buf,4,0);
			size = atoi(buf);
			root.debug(std::to_string(size));
			char line[size];
			rc = recv(controlSocket,line,size,0);
			line[rc] = '\0';
			std::string serverRequested(line);
			root.debug(serverRequested);
			root.debug("Server name recieved");
			for( std::vector<MinecraftServerService::Server*>::iterator i = servers->begin() ; i != servers->end(); i++) {
				root.debug((*i)->getServerName());
				if ((*i)->getServerName() == serverRequested) {
					root.debug("Found server");
					rc=recv(controlSocket,buf,4,0);
					size = atoi(buf);
					root.debug(std::to_string(size));
					char buff[size];
					rc = recv(controlSocket,buff,size,0);
					buff[rc] = '\0';
					std::string serverCommand(buff);
					root.debug("Server command recieved");
					root.debug(serverCommand);
					(*i)->sendCommand(serverCommand);
					writeToSocket("success", controlSocket, root);
					break;
				}
			}
		} else if (command == "listOnlinePlayers") {
			root.debug("Command recieved:  listOnlinePlayers");
			rc=recv(controlSocket,buf,4,0);
			size = atoi(buf);
			root.debug(std::to_string(size));
			char line[size];
			rc = recv(controlSocket,line,size,0);
			line[rc] = '\0';
			std::string serverRequested(line);
			root.debug("Server name recieved");
			for( std::vector<MinecraftServerService::Server*>::iterator i = servers->begin() ; i != servers->end(); i++) {
				root.debug((*i)->getServerName());
				if ((*i)->getServerName() == serverRequested) {
					(*i)->listOnlinePlayers();
					break;
				}
			}
		} else if (command == "listOnlinePlayersFiltered") {
			root.debug("Command recieved:  listOnlinePlayersFiltered");
			rc=recv(controlSocket,buf,4,0);
			size = atoi(buf);
			root.debug(std::to_string(size));
			char line[size];
			rc = recv(controlSocket,line,size,0);
			line[rc] = '\0';
			std::string serverRequested(line);
			root.debug("Server name recieved");
			for( std::vector<MinecraftServerService::Server*>::iterator i = servers->begin() ; i != servers->end(); i++) {
				root.debug((*i)->getServerName());
				if ((*i)->getServerName() == serverRequested) {
					rc=recv(controlSocket,buf,4,0);
					size = atoi(buf);
					root.debug(std::to_string(size));
					char buff[size];
					rc = recv(controlSocket,buff,size,0);
					buff[rc] = '\0';
					std::string playerName(buff);
					root.debug("playerName recieved");
					root.debug(playerName);
					(*i)->listOnlinePlayers(playerName);
					break;
				}
			}
		} else if (command == "updateServer") {
			root.debug("Command recieved:  updateServer");
			rc=recv(controlSocket,buf,4,0);
			size = atoi(buf);
			root.debug(std::to_string(size));
			char line[size];
			rc = recv(controlSocket,line,size,0);
			line[rc] = '\0';
			std::string serverRequested(line);
			root.debug("Server name recieved");
			for( std::vector<MinecraftServerService::Server*>::iterator i = servers->begin() ; i != servers->end(); i++) {
				root.debug((*i)->getServerName());
				if ((*i)->getServerName() == serverRequested) {
					(*i)->updateServer();
					writeToSocket("success", controlSocket, root);
					break;
				}
			}
		} else if (command == "updateAll") {
			root.debug("Command recieved:  updateAll");
			for( std::vector<MinecraftServerService::Server*>::iterator i = servers->begin() ; i != servers->end(); i++) {
				(*i)->updateServer();
			}
			writeToSocket("success", controlSocket, root);
		} else if (command == "backupServer") {
			root.debug("Command recieved:  backupServer");
			rc=recv(controlSocket,buf,4,0);
			size = atoi(buf);
			root.debug(std::to_string(size));
			char line[size];
			rc = recv(controlSocket,line,size,0);
			line[rc] = '\0';
			std::string serverRequested(line);
			root.debug("Server name recieved");
			for( std::vector<MinecraftServerService::Server*>::iterator i = servers->begin() ; i != servers->end(); i++) {
				if ((*i)->getServerName() == serverRequested) {
					(*i)->backupServer();
					writeToSocket("success", controlSocket, root);
					break;
				}
			}
		} else if (command == "backupAll") {
			root.debug("Command recieved:  backupAll");
			for( std::vector<MinecraftServerService::Server*>::iterator i = servers->begin() ; i != servers->end(); i++) {
				(*i)->backupServer();
			}
			writeToSocket("success", controlSocket, root);
		} else if (command == "stopDaemon") {
			root.debug("Command recieved:  stopDaemon");
			std::string serverList;
			for( std::vector<MinecraftServerService::Server*>::iterator i = servers->begin() ; i != servers->end(); i++) {
				root.debug((*i)->getServerName());
				(*i)->stopServer();
			}
			root.debug("All servers stopped");
			root.debug("Stopping");
			writeToSocket("Stopped", controlSocket, root);
			exit(0);
		}
	}
}
