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

#ifndef DAEMON_MAINLOOP_HPP_
#define DAEMON_MAINLOOP_HPP_

#include <event2/util.h>
#include <log4cpp/Category.hh>
#include <vector>

#include <Server/Server.hpp>

/**
 * Struct to contain the vector of server objects and a logging object.
 * This is passed to libevent so the callback function (recieveCommand) can perform its job.
 */
struct cb_data {
		std::vector<MinecraftServerDaemon::Server*>* servers;
		log4cpp::Category& root;
		struct event_base* base;
		struct event* event;
};

/**
 * Catches the SIGINT signal and kills all of the child processes, then sends the SIGINT signal again to kill the program.
 * @param sig
 */
void sigint_handler(int sig);
/**
 * Initializes the main control loop with libevent.
 * It creates an event that triggers whenever the control socket is ready for reading.  This event calls recieveCommend to do the processing.
 * @param servers
 * @param root
 * @param controlSocket
 * @param base
 */
void mainLoop(std::vector<MinecraftServerDaemon::Server*>* servers, log4cpp::Category& root, evutil_socket_t controlSocket, struct event_base*base);
/**
 * This function handles the processing of commands recieved from the control socket.  Called by libevent when the control socket is ready for reading.
 * @param _controlSocket
 * @param what
 * @param arg
 */
void acceptConnection(int controlSocket, short what, void *arg);
void readAndProcessMessage(int controlSocket, short what, void *arg);

#endif /* DAEMON_MAINLOOP_HPP_ */
