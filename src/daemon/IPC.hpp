/*******************************************************************************
 *
 * MinecraftServerDaemon
 * IPC.hpp
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

#ifndef DAEMON_IPC_HPP_
#define DAEMON_IPC_HPP_

#include <log4cpp/Category.hh>
#include <string>

/**
 * Creates the control socket used for IPC with the client.
 * @param root
 * @return
 */
int createSocket(log4cpp::Category& root);
/**
 * Writes the specified message to the control socket.
 * @param message
 * @param controlSocket
 * @param root
 */
void writeToSocket(std::string message, int controlSocket, log4cpp::Category& root);
/**
 * Reads from the control socket.
 * @param controlSocket
 * @param root
 * @return
 */
std::string readFromSocket(int controlSocket, log4cpp::Category& root);

#endif /* DAEMON_IPC_HPP_ */
