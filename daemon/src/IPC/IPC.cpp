/*******************************************************************************
 *
 * MinecraftServerDaemon
 * IPC.cpp
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

#include <fcntl.h>
#include <IPC/IPC.hpp>
#include <IPC/OutputMessage.hpp>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/writer.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <string>

/**
 * Creates the control socket used for IPC with the client.
 * @param root
 * @return
 */
int createSocket(log4cpp::Category& root) {
	int Socket;
	struct sockaddr_un local;
	if ((Socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		root.fatal("Failure creating socket");
		exit(1);
	}
	local.sun_family = AF_UNIX;
	strcpy(local.sun_path, "/etc/minecraft/control.socket");
	unlink(local.sun_path);
	int len;
	len = strlen(local.sun_path) + sizeof(local.sun_family);
	if (bind(Socket, (struct sockaddr *) &local, len) == -1) {
		root.fatal("Failure binding socket to path");
		exit(1);
	}

	if (listen(Socket, 5) == -1) {
		root.fatal("Failure listening on socket");
		exit(1);
	}
	fcntl(Socket, F_SETFL, O_NONBLOCK);
	return Socket;
}
/**
 * Writes the specified message to the control socket.
 * @param message
 * @param controlSocket
 * @param root
 */
void writeToSocket(MinecraftServerDaemon::OutputMessage message, int controlSocket, log4cpp::Category& root) {
	Json::Value encodedMessage;
	Json::FastWriter writer;
	encodedMessage["mode"] = message.mode;
	encodedMessage["messageData"] = message.messageData;
	encodedMessage["server"] = message.server;
	encodedMessage["serverOutput"] = message.serverOutput;
	encodedMessage["command"] = message.command;
	encodedMessage["success"] = message.success;
	encodedMessage["failureReason"] = message.failureReason;
	std::string messageString(writer.write(encodedMessage));
	std::ostringstream ss;
	ss << std::setw(8) << std::setfill('0') << messageString.size();
	std::string buf = ss.str() + messageString;
	send(controlSocket, buf.c_str(), buf.size(), 0);
}
/**
 * Reads from the control socket.
 * @param controlSocket
 * @param root
 * @return
 */
MinecraftServerDaemon::InputMessage readFromSocket(int controlSocket, log4cpp::Category& root) {
	int rc;
	char buff[8];
	rc = recv(controlSocket, buff, 8, 0);
	MinecraftServerDaemon::InputMessage messageObject;
	if (rc == -1) {
		root.error("Read from control socket failed");
		messageObject.error = true;
	} else if (rc == 0) {
		root.info("Client disconnected from control socket");
		//close(controlSocket);
		messageObject.error = true;
	} else {
		int size = atoi(buff);
		char line[size];
		rc = recv(controlSocket, line, size, 0);
		line[rc] = '\0';
		root.debug(line);
		Json::Value decodedMessage;
		Json::Reader reader;
		try {
			reader.parse(line, decodedMessage);
		} catch (...) {
		}
		messageObject.error = false;
		messageObject.command = decodedMessage["command"].asString();
		messageObject.server = decodedMessage["server"].asString();
		messageObject.player = decodedMessage["player"].asString();
		messageObject.serverCommand = decodedMessage["serverCommand"].asString();
		messageObject.version = decodedMessage["version"].asString();
		messageObject.reason = decodedMessage["reason"].asString();
		messageObject.script = decodedMessage["script"].asString();
	}
	return messageObject;
}
