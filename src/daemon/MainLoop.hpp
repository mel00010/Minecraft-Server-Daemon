/*******************************************************************************
 *
 * Minecraft Server Daemon
 * MainLoop.hpp
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

#ifndef MAINLOOP_H
#define MAINLOOP_H

#include <event2/util.h>
#include <string>
#include <vector>

#include "Server.hpp"

void sigint_handler(int sig);
void mainLoop (std::vector<MinecraftServerDaemon::Server*>* servers, log4cpp::Category& root, evutil_socket_t controlSocket, struct event_base*base);
void writeToSocket(std::string message, int controlSocket, log4cpp::Category& root);
std::string readFromSocket(int controlSocket, log4cpp::Category& root);
void recieveCommand(int controlSocket, short what, void *arg);
#endif /* MAINLOOP_H */
