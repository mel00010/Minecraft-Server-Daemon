/*
 * client.cpp
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
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

int controlSocket, rc, len;
char buff[4];

void help(char* argv0)
{
	std::cout << "Usage:  " << argv0 << " {list|start <SERVER>|stop <SERVER>|restart <SERVER>|sendcommand <SERVER> <COMMAND>|status <SERVER>|update <SERVER>|backup <SERVER>|listplayers <SERVER> [PLAYER]|stopall|restartall|updateall|backupall|stopdaemon}" << std::endl;
}
void writeToSocket(std::string command)
{
	std::ostringstream ss;
	ss << std::setw(4) << std::setfill('0') << command.size();
	std::string buf = ss.str() + command;
	send(controlSocket, buf.c_str(), buf.size(), 0);
}
std::string readFromSocket()
{
	rc=recv(controlSocket,buff,4,0);
	int size = atoi(buff);
	char line[size];
	rc = recv(controlSocket,line,size,0);
	line[rc] = '\0';
	return std::string(line);
}
int main(int argc, char *argv[])
{
	if (argc > 1)
	{
		std::string option(argv[1]);
		if (option == "--help") {
			help(argv[0]);
		} else if (option == "startdaemon") {
			if (system("minecraftd")==0)
			{
				std::cout << "Daemon started" << std::endl;
				return 0;
			} else {
				std::cout << "Daemon failed to start" << std::endl;
				return 1;
			}
		} else {
			struct sockaddr_un remote;	
			if ((controlSocket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
				std::cerr << "Failure creating socket" << std::endl;
				exit(1);
			}
		
			remote.sun_family = AF_UNIX;
			strcpy(remote.sun_path, "/etc/minecraft/control.socket");
			len = strlen(remote.sun_path) + sizeof(remote.sun_family);
			if (connect(controlSocket, (struct sockaddr *)&remote, len) == -1) {
				std::cerr << "Failure connecting to socket" << std::endl;
				exit(1);
			}
			if (option == "list"){
				writeToSocket("listServers");
				//~ sleep(1);
				std::string response(readFromSocket());
				std::cout << response << std::endl;
			} else if (option == "start") {
				if (argc > 2){
					writeToSocket("startServer");
					writeToSocket(argv[2]);
					//~ sleep(1);
					
					std::cout << readFromSocket() << std::endl;
				}
			} else if (option == "stop") {
				if (argc > 2){
					writeToSocket("stopServer");
					writeToSocket(argv[2]);
					//~ sleep(1);
					std::cout << readFromSocket() << std::endl;
				}
			} else if (option == "restart") {
				if (argc > 2){
					writeToSocket("restartServer");
					writeToSocket(argv[2]);
					//~ sleep(1);
					std::cout << readFromSocket() << std::endl;
				}
			} else if (option == "sendcommand") {
				if (argc > 3){
					writeToSocket("sendCommand");
					writeToSocket(argv[2]);
					writeToSocket(argv[3]);
					//~ sleep(1);
					std::cout << readFromSocket() << std::endl;
				}
			} else if (option == "status") {
				if (argc > 2){
					writeToSocket("serverStatus");
					writeToSocket(argv[2]);
					//~ sleep(1);
					std::cout << readFromSocket() << std::endl;
				}
			} else if (option == "update") {
				if (argc > 2){
					writeToSocket("updateServer");
					writeToSocket(argv[2]);
					//~ sleep(1);
					std::cout << readFromSocket() << std::endl;
				}
			} else if (option == "backup") {
				if (argc > 2){
					writeToSocket("backupServer");
					writeToSocket(argv[2]);
					//~ sleep(1);
					std::cout << readFromSocket() << std::endl;
				}
			} else if (option == "listplayers") {
				if (argc > 3){
					writeToSocket("listOnlinePlayersFiltered");
					writeToSocket(argv[2]);
					writeToSocket(argv[3]);
				} else if (argc > 2){
					writeToSocket("listOnlinePlayers");
					writeToSocket(argv[2]);
					//~ sleep(1);
					std::cout << readFromSocket() << std::endl;
				}
			} else if (option == "stopall") {
				writeToSocket("stopAll");
				//~ sleep(1);
				std::cout << readFromSocket() << std::endl;
			} else if (option == "restartall") {
				writeToSocket("restartAll");
				//~ sleep(1);
				std::cout << readFromSocket() << std::endl;
			} else if (option == "updateall") {
				writeToSocket("updateAll");
				//~ sleep(1);
				std::cout << readFromSocket() << std::endl;
			} else if (option == "backupall") {
				writeToSocket("backupAll");
				//~ sleep(1);
				std::cout << readFromSocket() << std::endl;
			} else if (option == "stopdaemon") {
				writeToSocket("stopDaemon");
				//~ sleep(1);
				std::cout << readFromSocket() << std::endl;
			} else {
				std::cout << "Error:  argument " << option << " not found" << std::endl;
				help(argv[0]);
				return 1;
			}
			close(controlSocket);
			return 0;
		}
	} else {
		help(argv[0]);
		return 1;
	}
}
