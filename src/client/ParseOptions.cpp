/*******************************************************************************
 *
 * Minecraft Server Daemon
 * ParseOptions.cpp
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

#include <Help.hpp>
#include <Message.hpp>
#include <OutputMessage.hpp>
#include <ParseOptions.hpp>
#include <Socket.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

void parseOptions(int argc, char *argv[]) {
	int Socket = openSocket();
	std::string option(argv[1]);
	OutputMessage output;
	if (option == "list") {
		output.command = "listServers";
	} else if (option == "start") {
		if (argc > 2) {
			output.command = "startServer";
			output.server = argv[2];
		}
	} else if (option == "stop") {
		if (argc > 2) {
			output.command = "stopServer";
			output.server = argv[2];
		}
	} else if (option == "restart") {
		if (argc > 2) {
			output.command = "restartServer";
			output.server = argv[2];
		}
	} else if (option == "sendcommand") {
		if (argc > 3) {
			output.command = "sendCommand";
			output.server = argv[2];
			output.serverCommand = argv[3];
		}
	} else if (option == "status") {
		if (argc > 2) {
			output.command = "serverStatus";
			output.server = argv[2];
		}
	} else if (option == "update") {
		if (argc > 2) {
			output.command = "updateServer";
			output.server = argv[2];
		}
	} else if (option == "backup") {
		if (argc > 2) {
			output.command = "backupServer";
			output.server = argv[2];
		}
	} else if (option == "listplayers") {
		if (argc > 3) {
			output.command = "listOnlinePlayersFiltered";
			output.server = argv[2];
			output.player = argv[3];
		} else if (argc > 2) {
			output.command = "listOnlinePlayers";
			output.server = argv[2];
		}
	} else if (option == "stopall") {
		output.command = "stopAll";
	} else if (option == "restartall") {
		output.command = "restartAll";
	} else if (option == "updateall") {
		output.command = "updateAll";
	} else if (option == "backupall") {
		output.command = "backupAll";
	} else if (option == "stopdaemon") {
		output.command = "stopDaemon";
	} else if (option == "op") {
		if (argc > 3) {
			output.command = "opPlayer";
			output.server = argv[2];
			output.player = argv[3];
		}
	} else if (option == "deop") {
		if (argc > 3) {
			output.command = "deopPlayer";
			output.server = argv[2];
			output.player = argv[3];
		}
	} else if (option == "ban") {
		if (argc > 4) {
			output.command = "banPlayerReason";
			output.server = argv[2];
			output.player = argv[3];
			output.reason = argv[4];
		} else if (argc > 3) {
			output.command = "banPlayer";
			output.server = argv[2];
			output.player = argv[3];
		}
	} else if (option == "pardon") {
		if (argc > 3) {
			output.command = "pardonPlayer";
			output.server = argv[2];
			output.player = argv[3];
		}
	} else if (option == "kick") {
		if (argc > 4) {
			output.command = "kickPlayerReason";
			output.server = argv[2];
			output.player = argv[3];
			output.reason = argv[4];
		} else if (argc > 3) {
			output.command = "kickPlayer";
			output.server = argv[2];
			output.player = argv[3];
		}
	} else {
		std::cout << "Error:  argument " << option << " not found" << std::endl;
		help(argv[0]);
		exit(1);
	}
	writeToSocket(output, Socket);
	Message message = readFromSocket(Socket);
	std::cout << message.messageData << std::endl;
	closeSocket(Socket);
}
