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

#include <Server/Player.hpp>
#include <Server/Server.hpp>


namespace MinecraftServerDaemon {

Player::Player(std::string _playerName, std::string _ipAddress, std::string _serverName, Server* _server) {
	playerName = _playerName;
	ipAddress = _ipAddress;
	serverName = _serverName;
	server = _server;
}
Player::~Player() {
	server = nullptr;
}
/**
 * Bans a player.
 */
void Player::ban() {
	(*server) << "ban " << playerName << std::endl;
}
/**
 * Bans a player with a reason.
 * @param reason
 */
void Player::ban(std::string reason) {
	(*server) << "ban " << playerName << " " << reason << std::endl;
}
/**
 * Pardons a player.
 * @param playerName
 */
void Player::pardon(std::string playerName) {
	(*server) << "pardon " << playerName << std::endl;
}
/**
 * Kicks a player.
 */
void Player::kick() {
	(*server) << "kick " << playerName << std::endl;
}
/**
 * Kicks a player with a reason.
 * @param reason
 */
void Player::kick(std::string reason) {
	(*server) << "kick " << playerName << " " << reason << std::endl;
}
/**
 * Ops a player.
 */
void Player::op() {
	(*server) << "op " << playerName << std::endl;
}
/**
 * Deops a player.
 */
void Player::deop() {
	((Server&) *server) << "deop " << playerName << std::endl;
}

} /* namespace MinecraftServerDaemon */
