/*******************************************************************************
 *
 * Minecraft Server Daemon
 * CreateSocket.cpp
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

#include <CreateSocket.hpp>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>

int createSocket(log4cpp::Category& root)
{
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
