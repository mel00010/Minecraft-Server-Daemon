/*******************************************************************************
 *
 * Minecraft Server Daemon
 * Player.hpp
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

#ifndef DAEMON_PLAYER_HPP_
#define DAEMON_PLAYER_HPP_

#include <string>
namespace MinecraftServerDaemon {
class Server;
} /* namespace MinecraftServerDaemon */

namespace MinecraftServerDaemon {
/**
 * Class representing a player.
 */
class Player {
	public:
		Player(std::string playerName, std::string ipAddress, std::string serverName, Server* server);
		virtual ~Player();
		/**
		 * Bans a player.
		 */
		void ban();
		/**
		 * Bans a player with a reason.
		 * @param reason
		 */
		void ban(std::string reason);
		/**
		 * Pardons a player.
		 * @param playerName
		 */
		void pardon(std::string playerName);
		/**
		 * Kicks a player.
		 */
		void kick();
		/**
		 * Kicks a player with a reason.
		 * @param reason
		 */
		void kick(std::string reason);
		/**
		 * Ops a player.
		 */
		void op();
		/**
		 * Deops a player.
		 */
		void deop();
		/**
		 * Returns the name of the player represented by the object.
		 * @return
		 */
		std::string getPlayerName() {
			return playerName;
		}
	public:
		/**
		 * Holds the name of the player.
		 */
		std::string playerName;
		/**
		 * Holds the ip address of the player.
		 */
		std::string ipAddress;
		/**
		 * Holds the server the player is on.
		 */
		std::string serverName;

		/**
		 * Holds the server object used for commands.
		 */
		Server* server;
};

} /* namespace MinecraftServerDaemon */

#endif /* DAEMON_PLAYER_HPP_ */
