#include "server.h"
#include "parser.h"
#include "mainLoop.h"
#include "setupServers.h"
#include <json/json.h>
#include "log4cpp/Category.hh"
#include <log4cpp/PropertyConfigurator.hh>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
struct cb_data
{
	std::vector<Server*>* servers;
	log4cpp::Category& root;
};
void cb(int sock, short what, void *arg)
{
    struct event_base *base = (event_base *)arg;
    event_base_loopbreak(base);
}
void mainLoop (std::vector<Server*>* servers, log4cpp::Category& root, int controlSocket, struct event_base* base) 
{
	root.info("Starting main loop");
	struct event *ev1;
	//~ struct event *ev2;
	cb_data _data{servers, root};
	cb_data* data = &_data;
	ev1 = event_new(base, controlSocket, EV_READ|EV_PERSIST|EV_ET, recieveCommand, data);
	//~ ev2 = event_new(base, controlSocket, EV_WRITE|EV_PERSIST, wrt, recieveCommand
		//~ (char*)"Writing event");

	event_add(ev1, NULL);
	//~ event_add(ev2, NULL);
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
	struct cb_data *data = (cb_data *)arg;
	std::vector<Server*>* servers = data->servers;
	log4cpp::Category& root = data->root;
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
			for( Server* i : *servers) {
				root.debug(i->serverName);
				if (i->serverName == serverRequested) {
					i->startServer();
					//~ root.debug("Server starting not implemented yet");
					writeToSocket("success", controlSocket, root);
					break;
				}
			}
		} else if (command == "startAll") {
			root.debug("Command recieved:  startAll");
			for( Server* i : *servers) {
				i->startServer();
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
			for( Server* i : *servers) {
				root.debug(i->serverName);
				if (i->serverName == serverRequested) {
					root.debug("Found server");
					if (i->running)
					{
						i->stopServer();
						if (!i->running)
						{
							writeToSocket("success", controlSocket, root);
						} else {
							writeToSocket("failure", controlSocket, root);
						}
						break;
					} else {
						writeToSocket(i->serverName+"not running", controlSocket, root);
					}
				}
			}
		} else if (command == "stopAll") {
			root.debug("Command recieved:  stopAll");
			for( Server* i : *servers) {
				i->stopServer();
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
			for( Server* i : *servers) {
				root.debug(i->serverName);
				if (i->serverName == serverRequested) {
					i->restartServer();
					writeToSocket("success", controlSocket, root);
					break;
				}
			}
		} else if (command == "restartAll") {
			root.debug("Command recieved:  restartAll");
			for( Server* i : *servers) {
				i->restartServer();
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
			for( Server* i : *servers) {
				root.debug(i->serverName);
				if (i->serverName == serverRequested) {
					root.warn("Status not implemented yet");
					break;
				}
			}
		} else if (command == "listServers") {
			root.debug("Command recieved:  listServers");
			std::string serverList;
			for( Server* i : *servers) {
				root.debug(i->serverName);
				serverList = serverList+i->serverName + "\n";
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
			for( Server* i : *servers) {
				root.debug(i->serverName);
				if (i->serverName == serverRequested) {
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
					i->sendCommand(serverCommand);
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
			for( Server* i : *servers) {
				root.debug(i->serverName);
				if (i->serverName == serverRequested) {
					i->listOnlinePlayers();
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
			for( Server* i : *servers) {
				root.debug(i->serverName);
				if (i->serverName == serverRequested) {
					rc=recv(controlSocket,buf,4,0);
					size = atoi(buf);
					root.debug(std::to_string(size));
					char buff[size];
					rc = recv(controlSocket,buff,size,0);
					buff[rc] = '\0';
					std::string playerName(buff);
					root.debug("playerName recieved");
					root.debug(playerName);
					i->listOnlinePlayers(playerName);
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
			for( Server* i : *servers) {
				root.debug(i->serverName);
				if (i->serverName == serverRequested) {
					i->updateServer();
					writeToSocket("success", controlSocket, root);
					break;
				}
			}
		} else if (command == "updateAll") {
			root.debug("Command recieved:  updateAll");
			for( Server* i : *servers) {
				i->updateServer();
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
			for( Server* i : *servers) {
				if (i->serverName == serverRequested) {
					i->backupServer();
					writeToSocket("success", controlSocket, root);
					break;
				}
			}
		} else if (command == "backupAll") {
			root.debug("Command recieved:  backupAll");
			for( Server* i : *servers) {
				i->backupServer();
			}
			writeToSocket("success", controlSocket, root);
		} else if (command == "stopDaemon") {
			root.debug("Command recieved:  stopDaemon");
			std::string serverList;
			for( Server* i : *servers) {
				root.debug(i->serverName);
				i->stopServer();
			}
			root.debug("All servers stopped");
			root.debug("Stopping");
			writeToSocket("Stopped", controlSocket, root);
			exit(0);
		}
	}
}
