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
#include <ParseOptions.hpp>
#include <Socket.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

void parseOptions(int argc, char *argv[]) {
	int Socket = openSocket();
	std::string option(argv[1]);
	if (option == "list") {
		writeToSocket("listServers", Socket);
		std::cout << readFromSocket(Socket) << std::endl;
	} else if (option == "start") {
		if (argc > 2) {
			writeToSocket("startServer", Socket);
			writeToSocket(argv[2], Socket);
			//~ sleep(1);

			std::cout << readFromSocket(Socket) << std::endl;
		}
	} else if (option == "stop") {
		if (argc > 2) {
			writeToSocket("stopServer", Socket);
			writeToSocket(argv[2], Socket);
			//~ sleep(1);
			std::cout << readFromSocket(Socket) << std::endl;
		}
	} else if (option == "restart") {
		if (argc > 2) {
			writeToSocket("restartServer", Socket);
			writeToSocket(argv[2], Socket);
			//~ sleep(1);
			std::cout << readFromSocket(Socket) << std::endl;
		}
	} else if (option == "sendcommand") {
		if (argc > 3) {
			writeToSocket("sendCommand", Socket);
			writeToSocket(argv[2], Socket);
			writeToSocket(argv[3], Socket);
			//~ sleep(1);
			std::cout << readFromSocket(Socket) << std::endl;
		}
	} else if (option == "status") {
		if (argc > 2) {
			writeToSocket("serverStatus", Socket);
			writeToSocket(argv[2], Socket);
			//~ sleep(1);
			std::cout << readFromSocket(Socket) << std::endl;
		}
	} else if (option == "update") {
		if (argc > 2) {
			writeToSocket("updateServer", Socket);
			writeToSocket(argv[2], Socket);
			//~ sleep(1);
			std::cout << readFromSocket(Socket) << std::endl;
		}
	} else if (option == "backup") {
		if (argc > 2) {
			writeToSocket("backupServer", Socket);
			writeToSocket(argv[2], Socket);
			//~ sleep(1);
			std::cout << readFromSocket(Socket) << std::endl;
		}
	} else if (option == "listplayers") {
		if (argc > 3) {
			writeToSocket("listOnlinePlayersFiltered", Socket);
			writeToSocket(argv[2], Socket);
			writeToSocket(argv[3], Socket);
		} else if (argc > 2) {
			writeToSocket("listOnlinePlayers", Socket);
			writeToSocket(argv[2], Socket);
			//~ sleep(1);
			std::cout << readFromSocket(Socket) << std::endl;
		}
	} else if (option == "stopall") {
		writeToSocket("stopAll", Socket);
		//~ sleep(1);
		std::cout << readFromSocket(Socket) << std::endl;
	} else if (option == "restartall") {
		writeToSocket("restartAll", Socket);
		//~ sleep(1);
		std::cout << readFromSocket(Socket) << std::endl;
	} else if (option == "updateall") {
		writeToSocket("updateAll", Socket);
		//~ sleep(1);
		std::cout << readFromSocket(Socket) << std::endl;
	} else if (option == "backupall") {
		writeToSocket("backupAll", Socket);
		//~ sleep(1);
		std::cout << readFromSocket(Socket) << std::endl;
	} else if (option == "stopdaemon") {
		writeToSocket("stopDaemon", Socket);
		//~ sleep(1);
		std::cout << readFromSocket(Socket) << std::endl;
	} else {
		std::cout << "Error:  argument " << option << " not found" << std::endl;
		help(argv[0]);
		exit(1);
	}
	closeSocket(Socket);
}
