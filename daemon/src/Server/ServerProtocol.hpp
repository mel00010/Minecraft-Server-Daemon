/*******************************************************************************
 *
 * MinecraftServerDaemon
 * ServerProtocol.hpp
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

#ifndef DAEMON_SRC_SERVER_SERVERPROTOCOL_HPP_
#define DAEMON_SRC_SERVER_SERVERPROTOCOL_HPP_
#include <string>

namespace MinecraftServerDaemon {
class ServerConnection {
	public:
		ServerConnection(std::string host, short port = 25565);
		uint64_t ping();
		bool query();
		void status();

		std::string host;
		short port;
		uint64_t latency;

	protected:
		bool sendPing();
		bool sendQuery();
		bool sendStatus();
		void tcpHandshake();
		void udpHandshake();
		unsigned char* createQueryPacket(unsigned char id);
		unsigned char* encodeVarInt(long int value);
		unsigned char* encodeVarLong(long long int value);
		long int decodeVarInt(unsigned char* data, long int* currentByte);
		long long int decodeVarLong(unsigned char* data, long int* currentByte);

	private:
		int connection=-1;
		long int challenge=-1;

};
} /* namespace MinecraftServerDaemon */

#endif /* DAEMON_SRC_SERVER_SERVERPROTOCOL_HPP_ */
