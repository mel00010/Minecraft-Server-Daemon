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

#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>
#include <jsoncpp/json/value.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <Socket.hpp>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

int openSocket() {
	int Socket;
	struct sockaddr_un remote;
	if ((Socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		std::cerr << "Failure creating socket" << std::endl;
		exit(1);
	}

	remote.sun_family = AF_UNIX;
//	strcpy(remote.sun_path, "/etc/minecraft/control.socket");
	strcpy(remote.sun_path, "/home/mel/workspace/web-client/socket.socket");

	int len = strlen(remote.sun_path) + sizeof(remote.sun_family);
	if (connect(Socket, (struct sockaddr *) &remote, len) == -1) {
		std::cerr << "Failure connecting to socket" << std::endl;
		exit(1);
	}
	return Socket;
}
void closeSocket(int Socket) {
	close(Socket);
}
void writeToSocket(OutputMessage command, int Socket) {
	Json::Value encodedMessage;
	Json::FastWriter writer;
	encodedMessage["command"] = command.command;
	encodedMessage["server"] = command.server;
	encodedMessage["player"] = command.player;
	encodedMessage["serverCommand"] = command.serverCommand;
	encodedMessage["version"] = command.version;
	encodedMessage["reason"] = command.reason;
	std::string messageString(writer.write(encodedMessage));
	std::ostringstream ss;
	ss << std::setw(8) << std::setfill('0') << messageString.size();
	std::string buf = ss.str() + messageString;
	send(Socket, buf.c_str(), buf.size(), 0);
}
Message readFromSocket(int Socket) {
	int rc;
	char buff[8];
	rc = recv(Socket, buff, 8, 0);
	Message messageObject;
	if (rc == -1) {
		std::cout << "Read from control socket failed" << std::endl;
		messageObject.error = true;
	} else if (rc == 0) {
		std::cout << "Server disconnected from control socket" << std::endl;
		close(Socket);
		messageObject.error = true;
	} else {
		int size = atoi(buff);
		char line[size];
		rc = recv(Socket, line, size, 0);
		line[rc] = '\0';
		Json::Value decodedMessage;
		Json::Reader reader;
		try {
			reader.parse(line, decodedMessage);
		} catch (...) {
		}
		messageObject.error = false;
		messageObject.mode = decodedMessage["mode"].asString();
		messageObject.messageData = decodedMessage["messageData"].asString();
	}
	return messageObject;
}
