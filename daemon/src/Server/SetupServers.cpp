/*******************************************************************************
 *
 * Minecraft Server Daemon
 * SetupServers.cpp
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

#include <jsoncpp/json/forwards.h>
#include <Server/BukkitServer.hpp>
#include <Server/BungeeCordServer.hpp>
#include <Server/ForgeServer.hpp>
#include <Server/GenericServer.hpp>
#include <Server/SpigotServer.hpp>
#include <Server/VanillaServer.hpp>
#include <Server/SetupServers.hpp>
#include <cstdlib>
#include <string>

/**
 * Sets up servers using the data from the daemon config file.
 * @param _config
 * @param log
 * @return
 */
std::vector<MinecraftServerDaemon::Server*>* setupServers(Json::Value* _config, log4cpp::Category& log) {
	Json::Value config = *_config;
	std::vector<MinecraftServerDaemon::Server*>* servers = new std::vector<MinecraftServerDaemon::Server*>;
	std::string defaultServerDirectory = config["options"]["defaultServerDirectory"].asString();
	log.notice(defaultServerDirectory);
	for (Json::Value::iterator itr = config["servers"].begin(); itr != config["servers"].end(); itr++) {
		Json::Value server = (*itr);
		std::string serverName = server["serverName"].asString();
		std::string serverPathBuf = server["serverPath"].asString();
		std::string serverPath;
		log.notice(std::to_string(serverPathBuf.at(0)));
		log.notice(std::to_string('/'));
		if (serverPathBuf.at(0) == '/') {
			serverPath = serverPathBuf;
			log.notice(serverPath);
		} else {
			serverPath = defaultServerDirectory + "/" + serverPathBuf;
			log.notice(serverPath);
		}
		std::string serverJarName = server["serverJarName"].asString();
		std::string serverAccount = server["serverAccount"].asString();
		int maxHeapAlloc = server["maxHeapAlloc"].asInt();
		int minHeapAlloc = server["minHeapAlloc"].asInt();
		int gcThreadCount = server["gcThreadCount"].asInt();
		std::vector<std::string> javaArgs;
		for (Json::Value::iterator itr = server["javaArgs"].begin(); itr != server["javaArgs"].end(); itr++) {
			Json::Value arg = (*itr);
			javaArgs.push_back((const std::string) arg.asString());
		}
		std::vector<std::string> serverOptions;
		for (Json::Value::iterator itr = server["serverOptions"].begin(); itr != server["serverOptions"].end(); itr++) {
			Json::Value option = (*itr);
			serverOptions.push_back((const std::string) option.asString());
		}
		std::string serverType = server[serverType].asString();
		if (serverType == "vanilla") {
			std::string backupPath = server["backupPath"].asString();
			std::vector<std::string> worldsToBackup;
			for (Json::Value::iterator itr = server["worldsToBackup"].begin(); itr != server["worldsToBackup"].end(); itr++) {
				Json::Value world = (*itr);
				worldsToBackup.push_back((const std::string) world.asString());
			}
			servers->push_back(
					new MinecraftServerDaemon::VanillaServer(serverName, serverPath, serverJarName, serverAccount, maxHeapAlloc, minHeapAlloc, gcThreadCount,
							backupPath, worldsToBackup, javaArgs, serverOptions));
			servers->back()->startServer();
		} else if (serverType == "spigot") {
			std::string backupPath = server["backupPath"].asString();
			std::vector<std::string> worldsToBackup;
			for (Json::Value::iterator itr = server["worldsToBackup"].begin(); itr != server["worldsToBackup"].end(); itr++) {
				Json::Value world = (*itr);
				worldsToBackup.push_back((const std::string) world.asString());
			}
			servers->push_back(
					new MinecraftServerDaemon::SpigotServer(serverName, serverPath, serverJarName, serverAccount, maxHeapAlloc, minHeapAlloc, gcThreadCount,
							backupPath, worldsToBackup, javaArgs, serverOptions));
			servers->back()->startServer();
		} else if (serverType == "bukkit") {
			std::string backupPath = server["backupPath"].asString();
			std::vector<std::string> worldsToBackup;
			for (Json::Value::iterator itr = server["worldsToBackup"].begin(); itr != server["worldsToBackup"].end(); itr++) {
				Json::Value world = (*itr);
				worldsToBackup.push_back((const std::string) world.asString());
			}
			servers->push_back(
					new MinecraftServerDaemon::BukkitServer(serverName, serverPath, serverJarName, serverAccount, maxHeapAlloc, minHeapAlloc, gcThreadCount,
							backupPath, worldsToBackup, javaArgs, serverOptions));
			servers->back()->startServer();
		} else if (serverType == "bungeecord") {
			std::string backupPath = server["backupPath"].asString();
			servers->push_back(
					new MinecraftServerDaemon::BungeeCordServer(serverName, serverPath, serverJarName, serverAccount, maxHeapAlloc, minHeapAlloc, gcThreadCount,
							backupPath, javaArgs, serverOptions));
			servers->back()->startServer();
		} else if (serverType == "forge") {
			std::string backupPath = server["backupPath"].asString();
			std::vector<std::string> worldsToBackup;
			for (Json::Value::iterator itr = server["worldsToBackup"].begin(); itr != server["worldsToBackup"].end(); itr++) {
				Json::Value world = (*itr);
				worldsToBackup.push_back((const std::string) world.asString());
			}
			servers->push_back(
					new MinecraftServerDaemon::ForgeServer(serverName, serverPath, serverJarName, serverAccount, maxHeapAlloc, minHeapAlloc, gcThreadCount,
							backupPath, worldsToBackup, javaArgs, serverOptions));
			servers->back()->startServer();
		} else if (serverType == "other") {
			std::string backupPath = server["backupPath"].asString();
			std::vector<std::string> worldsToBackup;
			for (Json::Value::iterator itr = server["worldsToBackup"].begin(); itr != server["worldsToBackup"].end(); itr++) {
				Json::Value world = (*itr);
				worldsToBackup.push_back((const std::string) world.asString());
			}
			servers->push_back(
					new MinecraftServerDaemon::GenericServer(serverName, serverPath, serverJarName, serverAccount, maxHeapAlloc, minHeapAlloc, gcThreadCount,
							backupPath, worldsToBackup, javaArgs, serverOptions));
			servers->back()->startServer();
		} else {
			log.fatal("No such server type " + serverType);
			exit(-1);
		}
	}
	return servers;
}
