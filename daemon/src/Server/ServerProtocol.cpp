/*******************************************************************************
 *
 * MinecraftServerDaemon
 * ServerProtocol.cpp
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

#include <netdb.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <Server/ServerProtocol.hpp>
#include <unistd.h>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <linux/random.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <errno.h>
#include <jsoncpp/json/writer.h>
#include <jsoncpp/json/reader.h>
#include <map>

namespace MinecraftServerDaemon {

ServerConnection::ServerConnection(std::string host, short port) :
		host { host }, port { port } {

}

unsigned char* ServerConnection::encodeVarInt(long int value) {
	unsigned char* buffer = new unsigned char[5];
	std::fill_n(buffer, 5, 0);
	unsigned long int uvalue = (unsigned long int) value;
	int encoded = 0;
	do {
		unsigned char next_byte = uvalue & 0x7F;
		uvalue >>= 7;
		if (uvalue != 0) {
			next_byte |= 0x80;
		}
		buffer[encoded++] = next_byte;
	} while (uvalue != 0);
	buffer[encoded++] = (unsigned char) NULL;
	return buffer;
}

unsigned char* ServerConnection::encodeVarLong(long long int value) {
	unsigned char* buffer = new unsigned char[10];
	std::fill_n(buffer, 10, 0);
	unsigned long int uvalue = (unsigned long int) value;
	int encoded = 0;
	do {
		unsigned char next_byte = uvalue & 0x7F;
		uvalue >>= 7;
		if (uvalue != 0) {
			next_byte |= 0x80;
		}
		buffer[encoded++] = next_byte;
	} while (uvalue != 0);
	buffer[encoded++] = (unsigned char) NULL;
	return buffer;
}

long int ServerConnection::decodeVarInt(unsigned char* data, long int* currentByte) {
	unsigned char buffer[5] = { 0 };
	memcpy(buffer, &data[*currentByte], 5);
	int numRead = 0;
	unsigned long int result = 0;
	unsigned char byte;
	do {
		byte = buffer[numRead];
		unsigned long int value = (byte & 0x7F);
		result |= (value << (7 * numRead));

		numRead++;
		if (numRead > 5) {
			std::cerr << "VarInt is too big" << std::endl;
			return false;
		}
	} while ((byte & 0x80) != 0);
	*currentByte = *currentByte + numRead;
	return (long int) result;
}

long long int ServerConnection::decodeVarLong(unsigned char* data, long int* currentByte) {
	unsigned char buffer[10] = { 0 };
	memcpy(buffer, &data[*currentByte], 10);
	int numRead = 0;
	unsigned long long int result = 0;
	unsigned char byte;
	do {
		byte = buffer[numRead];
		unsigned long long int value = (byte & 0x7F);
		result |= (value << (7 * numRead));

		numRead++;
		if (numRead > 10) {
			std::cerr << "VarLong is too big" << std::endl;
			return false;
		}
	} while ((byte & 0x80) != 0);

	*currentByte = *currentByte + numRead;
	return (long long int) result;
}

uint64_t ServerConnection::ping() {
	udpHandshake();
	sendPing();
	close(connection);
	return latency;
}

void ServerConnection::status() {
	tcpHandshake();
	sendStatus();
	sendPing();
	close(connection);
}

bool ServerConnection::query() {
	udpHandshake();
	sendQuery();
	close(connection);
	return true;
}

void ServerConnection::udpHandshake() {
	challenge = 0;
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int s;

	/* Obtain address(es) matching host/port */

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC; /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
	hints.ai_flags = 0;
	hints.ai_protocol = 0; /* Any protocol */

	s = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &result);
	if (s != 0) {
		std::cerr << "getaddrinfo: " << gai_strerror(s) << std::endl;
		exit(EXIT_FAILURE);
	}

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		connection = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (connection == -1)
			continue;

		if (connect(connection, rp->ai_addr, rp->ai_addrlen) != -1)
			break; /* Success */

		close(connection);
	}

	if (rp == NULL) { /* No address succeeded */
		std::cerr << "Could not connect" << std::endl;
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(result);
	unsigned char* packet = createQueryPacket(9);
	send(connection, packet, 11, 0);
	unsigned char recvdPacket[14] = { 0 };
	recv(connection, &recvdPacket[0], 13, 0);
	char challengeBuf[9] = { 0 };

	memcpy(&challengeBuf, &recvdPacket[5], 8);
	challenge = atoi((const char*) challengeBuf);
}

unsigned char* ServerConnection::createQueryPacket(unsigned char id) {
	unsigned char* buffer[4] = { 0 };
	unsigned char prefix[4] = { 0 };
	unsigned char idBuf[2] = { 0 };

	prefix[0] = 0xfe;
	prefix[1] = 0xfd;
	buffer[0] = (unsigned char*) prefix;
	idBuf[0] = id;
	buffer[1] = (unsigned char*) idBuf;
	buffer[2] = (unsigned char*) "\0\0\0\0";
	long int tmp = (challenge << 16) | ((challenge >> 16) & 0x00ffff);
	long int challengeBuf = ((tmp >> 8) & 0x00ff00ff) | ((tmp & 0x00ff00ff) << 8);

	buffer[3] = (unsigned char*) &challengeBuf;
	unsigned char packet[13] = { 0 };
	memcpy(&packet[0], buffer[0], 2);
	memcpy(&packet[2], buffer[1], 1);
	memcpy(&packet[3], buffer[2], 4);
	memcpy(&packet[7], buffer[3], 4);
	packet[12] = (unsigned char) NULL;
	unsigned char* finalPacket = new unsigned char[12];
	std::fill_n(finalPacket, 12, 0);
	memcpy(&finalPacket[0], packet, 12);
	return finalPacket;
}

void ServerConnection::tcpHandshake() {
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int s;

	/* Obtain address(es) matching host/port */

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC; /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
	hints.ai_flags = 0;
	hints.ai_protocol = 0; /* Any protocol */

	s = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &result);
	if (s != 0) {
		std::cerr << "getaddrinfo: " << gai_strerror(s) << std::endl;
		exit(EXIT_FAILURE);
	}

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		connection = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (connection == -1)
			continue;

		if (connect(connection, rp->ai_addr, rp->ai_addrlen) != -1)
			break; /* Success */

		close(connection);
	}

	if (rp == NULL) { /* No address succeeded */
		std::cerr << "Could not connect" << std::endl;
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(result);
	unsigned char* buffer[6] = { 0 };
	unsigned char portChar[4] = { 0 };
	buffer[0] = encodeVarInt(0);
	buffer[1] = encodeVarInt(47);
	buffer[2] = encodeVarInt(strlen("localhost"));
	buffer[3] = (unsigned char*) "localhost";
	portChar[0] = (unsigned char) (port >> 8);
	portChar[1] = (unsigned char) (port & 0xff);
	buffer[4] = (unsigned char*) portChar;
	buffer[5] = encodeVarInt(1);
	unsigned char packet[128] = { 0 };
	size_t startingIndex = 0;
	memcpy(&packet[startingIndex], buffer[0], 2);
	startingIndex = 1;

	for (size_t i = 1; i != 6; i++) {
		memcpy(&packet[startingIndex], buffer[i], strlen((char*) buffer[i]) + 1);
		startingIndex = startingIndex + strlen((char*) buffer[i]);
	}
	packet[startingIndex] = (unsigned char) NULL;
	unsigned char finalPacket[128] = { 0 };
	memcpy(&finalPacket[0], packet, startingIndex);

	unsigned char* packetLen = encodeVarInt(startingIndex);
	send(connection, packetLen, strlen((const char*) packetLen), 0);
	send(connection, finalPacket, startingIndex, 0);
}

bool ServerConnection::sendPing() {
	unsigned char* buffer[2] = { 0 };
	unsigned char packet[64] = { 0 };
	unsigned char recvBuffer[1024] = { 0 };
	struct timespec start, end;
	long long int token;
	buffer[0] = encodeVarInt(1);
	do {
		syscall(SYS_getrandom, &token, 8, 0);
	} while (token <= 0);
	buffer[1] = new unsigned char[8];
	memcpy(buffer[1], &token, 8);
	memcpy(&packet[0], buffer[0], 1);
	memcpy(&packet[1], buffer[1], 8);

	unsigned char* packetLen = encodeVarInt(9);

	clock_gettime(CLOCK_MONOTONIC, &start);
	send(connection, packetLen, 1, 0);
	send(connection, packet, 9, 0);
	int numRead = 0;
	long int resultLen = 0;
	unsigned char byte = 0;
	do {
		recv(connection, &byte, 1, 0);
		int value = (byte & 0x7F);
		resultLen |= (value << (7 * numRead));

		numRead++;
		if (numRead > 5) {
			std::cerr << "VarInt is too big" << std::endl;
			return false;
		}
	} while ((byte & 0x80) != 0);

	recv(connection, recvBuffer, resultLen, 0);
	clock_gettime(CLOCK_MONOTONIC, &end);

	long int currentByte = 0;
	long int recvMode = decodeVarInt(recvBuffer, &currentByte);
	if (recvMode != 1) {
		std::cerr << "Received mode is not 1!" << std::endl;
		std::cerr << std::hex << recvMode << std::endl;
		return false;
	}
	long long int recvToken;
	memcpy(&recvToken, &recvBuffer[currentByte], 8);
	if (recvToken != token) {
		std::cerr << "Received ping token does not match original!" << std::endl;
		std::cerr << "Received ping token base 10:  " << std::dec << recvToken << std::endl;
		std::cerr << "Original ping token base 10:  " << std::dec << token << std::endl;
		std::cerr << "Received ping token base 16:  " << std::hex << recvToken << std::endl;
		std::cerr << "Original ping token base 16:  " << std::hex << token << std::endl;
		return false;
	}

	uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
	latency = delta_us;
	return true;
}

bool ServerConnection::sendStatus() {
	unsigned char len = 0x01;
	unsigned char packetId = 0x00;
	send(connection, &len, 1, 0);
	send(connection, &packetId, 1, 0);

	int numRead = 0;
	long int resultLen = 0;
	unsigned char byte = 0;
	unsigned char recvBuffer[10240] = { 0 };
	do {
		recv(connection, &byte, 1, 0);
		int value = (byte & 0x7F);
		resultLen |= (value << (7 * numRead));

		numRead++;
		if (numRead > 5) {
			std::cerr << "VarInt is too big" << std::endl;
			return false;
		}
	} while ((byte & 0x80) != 0);
	std::cout << resultLen << std::endl;
	std::cout << recv(connection, recvBuffer, resultLen, 0) << std::endl;
	std::cout << std::setfill('0');
	for (int i = 0; i < resultLen; ++i) {
		std::cout << std::hex << std::setw(2) << (int) recvBuffer[i] << (((i + 1) % 16 == 0) ? "\n" : "");
	}

	std::cout << std::endl;

	std::cout << &recvBuffer[numRead + 1] << std::endl;

	return true;
}

bool ServerConnection::sendQuery() {
	unsigned char* queryBuf1 = createQueryPacket(0);
	unsigned char* queryBuf2 = (unsigned char*) "\0\0\0\0";
	unsigned char packet[16] = { 0 };
	memcpy(&packet[0], queryBuf1, 11);
	memcpy(&packet[11], queryBuf2, 4);
	packet[15] = (unsigned char) NULL;
	send(connection, packet, 15, 0);
	std::vector<unsigned char> responseBuf(4096);
	recv(connection, responseBuf.data(), responseBuf.size(), 0);
	responseBuf.shrink_to_fit();
	std::vector<char> buffer;
	size_t responseIndex = 16;
	std::map<std::string, std::string> data;
	std::vector<std::string> players;
	while (true) {
		while (buffer.empty() || (buffer.back() != '\0')) {
			buffer.push_back(responseBuf[responseIndex]);
			responseIndex++;
		}
		std::vector<char> key = buffer;
		if (key.empty() || (key.front() == '\0')) {
			responseIndex++;
			buffer.clear();
			break;
		}
		buffer.clear();
		while (buffer.empty() || (buffer.back() != '\0')) {
			buffer.push_back(responseBuf[responseIndex]);
			responseIndex++;
		}
		std::vector<char> value = buffer;
		buffer.clear();
		data[std::string(key.begin(), key.end())] = std::string(value.begin(), value.end());
	}
	responseIndex += 9;
	while (true) {
		while (buffer.empty() || (buffer.back() != '\0')) {
			buffer.push_back(responseBuf[responseIndex]);
			responseIndex++;
		}
		std::vector<char> name = buffer;
		buffer.clear();
		if (name.empty() || (name.front() == '\0')) {
			buffer.clear();
			break;
		}
		std::cerr << std::string(name.begin(), name.end()) << std::endl;
		players.push_back(std::string(name.begin(), name.end()));
	}
	std::cout << "Server info:  " << std::endl;
	for (auto& kv : data) {
		std::cout << "\t" << kv.first << ":    \t" << kv.second << std::endl;
	}
	if (players.empty() == false) {
		std::cout << "The following players are online:  " << std::endl;
		for (auto& i : players) {
			std::cout << "\t" << i << std::endl;
		}
	} else {
		std::cout << "There are no players on the server." << std::endl;
	}
	return true;
}

} /* namespace MinecraftServerDaemon */
