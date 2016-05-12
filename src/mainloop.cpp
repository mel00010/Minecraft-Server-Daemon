#include "server.h"
#include "parser.h"
#include "mainloop.h"
#include "setupservers.h"
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
void cb(int sock, short what, void *arg)
{
    struct event_base *base = (event_base *)arg;
    event_base_loopbreak(base);
}
void mainLoop (std::vector<Server*>* servers, log4cpp::Category& root) {
	char *pipePath = "/etc/minecraft/control.pipe";
	mkfifo(pipePath, 0666);
	char buf[4];
	int fd,rc;
	root.info("Opening pipe");
	fd = open(pipePath, O_RDONLY);
	root.info("Opened pipe");
	while (true) {
		while ( (rc=read(fd,buf,4)) > 0) {
			buf[rc] = '\0';
			int size = atoi(buf);
			root.debug(std::to_string(size));
			char line[size];
			rc = read(fd,line,size);
			line[rc] = '\0';
			root.debug("Client connected to control pipe");
			std::string command(line);
			root.debug(command);
			if (command == "startServer") {
				root.debug("Command recieved:  startServer");
				rc=read(fd,buf,4);
				size = atoi(buf);
				root.debug(std::to_string(size));
				char line[size];
				rc = read(fd,line,size);
				line[rc] = '\0';
				std::string serverRequested(line);
				root.debug("Server name recieved");
				for( Server* i : *servers) {
					root.debug(i->serverName);
					if (i->serverName == serverRequested) {
						i->startServer();
						//~ root.debug("Server starting not implemented yet");
						writeToPipe("success");
						break;
					}
				}
			} else if (command == "startAll") {
				root.debug("Command recieved:  startAll");
				for( Server* i : *servers) {
					i->startServer();
				}
				writeToPipe("success");
			} else if (command == "stopServer") {
				root.debug("Command recieved:  stopServer");
				rc=read(fd,buf,4);
				size = atoi(buf);
				root.debug(std::to_string(size));
				char line[size];
				rc = read(fd,line,size);
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
								writeToPipe("success");
							} else {
								writeToPipe("failure");
							}
							break;
						} else {
							writeToPipe(i->serverName+"not running");
						}
					}
				}
			} else if (command == "stopAll") {
				root.debug("Command recieved:  stopAll");
				for( Server* i : *servers) {
					i->stopServer();
				}
				writeToPipe("success");
			} else if (command == "restartServer") {
				root.debug("Command recieved:  restartServer");
				rc=read(fd,buf,4);
				size = atoi(buf);
				root.debug(std::to_string(size));
				char line[size];
				rc = read(fd,line,size);
				line[rc] = '\0';
				std::string serverRequested(line);
				root.debug("Server name recieved");
				for( Server* i : *servers) {
					root.debug(i->serverName);
					if (i->serverName == serverRequested) {
						i->restartServer();
						writeToPipe("success");
						break;
					}
				}
			} else if (command == "restartAll") {
				root.debug("Command recieved:  restartAll");
				for( Server* i : *servers) {
					i->restartServer();
				}
				writeToPipe("success");
			} else if (command == "serverStatus") {
				root.debug("Command recieved:  serverStatus");
				size = atoi(buf);
				root.debug(std::to_string(size));
				char line[size];
				rc = read(fd,line,size);
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
				writeToPipe(serverList);
			} else if (command == "sendCommand") {
				root.debug("Command recieved:  sendCommand");
				rc=read(fd,buf,4);
				size = atoi(buf);
				root.debug(std::to_string(size));
				char line[size];
				rc = read(fd,line,size);
				line[rc] = '\0';
				std::string serverRequested(line);
				root.debug(serverRequested);
				root.debug("Server name recieved");
				for( Server* i : *servers) {
					root.debug(i->serverName);
					if (i->serverName == serverRequested) {
						root.debug("Found server");
						rc=read(fd,buf,4);
						size = atoi(buf);
						root.debug(std::to_string(size));
						char buff[size];
						rc = read(fd,buff,size);
						buff[rc] = '\0';
						std::string serverCommand(buff);
						root.debug("Server command recieved");
						root.debug(serverCommand);
						i->sendCommand(serverCommand);
						writeToPipe("success");
						break;
					}
				}
			} else if (command == "listOnlinePlayers") {
				root.debug("Command recieved:  listOnlinePlayers");
				rc=read(fd,buf,4);
				size = atoi(buf);
				root.debug(std::to_string(size));
				char line[size];
				rc = read(fd,line,size);
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
				rc=read(fd,buf,4);
				size = atoi(buf);
				root.debug(std::to_string(size));
				char line[size];
				rc = read(fd,line,size);
				line[rc] = '\0';
				std::string serverRequested(line);
				root.debug("Server name recieved");
				for( Server* i : *servers) {
					root.debug(i->serverName);
					if (i->serverName == serverRequested) {
						rc=read(fd,buf,4);
						size = atoi(buf);
						root.debug(std::to_string(size));
						char buff[size];
						rc = read(fd,buff,size);
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
				rc=read(fd,buf,4);
				size = atoi(buf);
				root.debug(std::to_string(size));
				char line[size];
				rc = read(fd,line,size);
				line[rc] = '\0';
				std::string serverRequested(line);
				root.debug("Server name recieved");
				for( Server* i : *servers) {
					root.debug(i->serverName);
					if (i->serverName == serverRequested) {
						i->updateServer();
						writeToPipe("success");
						break;
					}
				}
			} else if (command == "updateAll") {
				root.debug("Command recieved:  updateAll");
				for( Server* i : *servers) {
					i->updateServer();
				}
				writeToPipe("success");
			} else if (command == "backupServer") {
				root.debug("Command recieved:  backupServer");
				rc=read(fd,buf,4);
				size = atoi(buf);
				root.debug(std::to_string(size));
				char line[size];
				rc = read(fd,line,size);
				line[rc] = '\0';
				std::string serverRequested(line);
				root.debug("Server name recieved");
				for( Server* i : *servers) {
					if (i->serverName == serverRequested) {
						i->backupServer();
						writeToPipe("success");
						break;
					}
				}
			} else if (command == "backupAll") {
				root.debug("Command recieved:  backupAll");
				for( Server* i : *servers) {
					i->backupServer();
				}
				writeToPipe("success");
			} else if (command == "stopDaemon") {
				root.debug("Command recieved:  stopDaemon");
				writeToPipe("Stopped");
				std::string serverList;
				for( Server* i : *servers) {
					root.debug(i->serverName);
					i->stopServer();
				}
				root.debug("All servers stopped");
				root.debug("Stopping");
				exit(0);
			}
		}
		fd = open(pipePath, O_RDONLY);
		if (rc == -1) {
		  root.error("Read from control pipe failed");
		}
		else if (rc == 0) {
			root.info("Client disconnected from control pipe");
		}
	}
}
void writeToPipe(std::string message)
{
	char *outputPipePath = "/etc/minecraft/output.pipe";
	mkfifo(outputPipePath, 0666);
	int fd = open(outputPipePath, O_WRONLY | O_NONBLOCK);
	std::ostringstream ss;
	ss << std::setw(4) << std::setfill('0') << message.size();
	std::string result = ss.str();
	std::string buf = ss.str() + message;
	write(fd, buf.c_str(), buf.size());
	close(fd);
}
