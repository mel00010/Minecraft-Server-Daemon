/*******************************************************************************
 *
 * Minecraft Server Daemon
 * Player.cpp
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

#include "Player.hpp"

#include "Server.hpp"

namespace MinecraftServerDaemon {

Player::Player(std::string playerName, std::string ipAddress, std::string serverName, Server* server) :
		playerName(playerName), ipAddress(ipAddress), serverName(serverName), server(server) {
}
Player::~Player() {
}
/**
 * Bans a player.
 */
void Player::ban() {
	((Server&) *server) << "ban " << playerName << std::endl;
}
/**
 * Bans a player with a reason.
 * @param reason
 */
void Player::ban(std::string reason) {
	((Server&) *server) << "ban " << playerName << " " << reason << std::endl;
}
/**
 * Pardons a player.
 * @param playerName
 */
void Player::pardon(std::string playerName) {
	((Server&) *server) << "pardon " << playerName << std::endl;
}
/**
 * Kicks a player.
 */
void Player::kick() {
	((Server&) *server) << "kick " << playerName << std::endl;
} /**
 * Kicks a player with a reason.
 * @param reason
 */
void Player::kick(std::string reason) {
	((Server&) *server) << "kick " << playerName << " " << reason << std::endl;
}
/**
 * Ops a player.
 */
void Player::op() {
	((Server&) *server) << "op " << playerName << std::endl;
}
/**
 * Deops a player.
 */
void Player::deop() {
	((Server&) *server) << "deop " << playerName << std::endl;
}

} /* namespace MinecraftServerDaemon */
