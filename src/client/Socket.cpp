/*******************************************************************************
 *
 * Minecraft Server Daemon
 * Socket.cpp
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

#include <sys/socket.h>
#include <sys/un.h>
#include <Socket.hpp>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>

int openSocket()
{
	int Socket;
	struct sockaddr_un remote;
	if ((Socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		std::cerr << "Failure creating socket" << std::endl;
		exit(1);
	}

	remote.sun_family = AF_UNIX;
	strcpy(remote.sun_path, "/etc/minecraft/control.socket");
	int len = strlen(remote.sun_path) + sizeof(remote.sun_family);
	if (connect(Socket, (struct sockaddr *)&remote, len) == -1) {
		std::cerr << "Failure connecting to socket" << std::endl;
		exit(1);
	}
	return Socket;
}
void closeSocket(int Socket)
{
	close(Socket);
}
void writeToSocket(std::string command, int Socket)
{
	std::ostringstream ss;
	ss << std::setw(4) << std::setfill('0') << command.size();
	std::string buf = ss.str() + command;
	send(Socket, buf.c_str(), buf.size(), 0);
}
std::string readFromSocket(int Socket)
{
	int rc;
	char buff[4];
	rc=recv(Socket,buff,4,0);
	int size = atoi(buff);
	char line[size];
	rc = recv(Socket,line,size,0);
	line[rc] = '\0';
	return std::string(line);
}
