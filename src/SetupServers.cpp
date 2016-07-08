/*
 * SetupServers.cpp
 * 
 * Copyright 2016 Mel McCalla <melmccalla@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
 
#include "SetupServers.hpp"
#include "AbstractServerBase.hpp"
#include "VanillaServer.hpp"
#include "SpigotServer.hpp"
#include "BukkitServer.hpp"
#include "BungeeCordServer.hpp"
#include "GenericServer.hpp"
#include <vector>
#include <json/json.h>
#include <string>
#include "log4cpp/Category.hh"
#include <log4cpp/PropertyConfigurator.hh>
std::vector<MinecraftServerService::Server*>* setupServers(Json::Value* _config, log4cpp::Category& log) {
	Json::Value config = *_config;
	std::vector<MinecraftServerService::Server*>* servers = new std::vector<MinecraftServerService::Server*>;
	for (Json::Value::iterator itr = config["servers"].begin(); itr != config["servers"].end(); itr++)
	{
		Json::Value server = (*itr);
		std::string serverName = server["serverName"].asString();
		std::string serverPath = server["serverPath"].asString();
		std::string serverJarName = server["serverJarName"].asString();
		std::string serverAccount = server["serverAccount"].asString();
		int maxHeapAlloc = server["maxHeapAlloc"].asInt();
		int minHeapAlloc = server["minHeapAlloc"].asInt();
		int gcThreadCount = server["gcThreadCount"].asInt();
		std::vector<std::string> javaArgs;
		for (Json::Value::iterator itr = server["javaArgs"].begin(); itr != server["javaArgs"].end(); itr++)
		{
			Json::Value arg = (*itr);
			javaArgs.push_back(arg.asString());
		}
		std::vector<std::string> serverOptions;
		for (Json::Value::iterator itr = server["serverOptions"].begin(); itr != server["serverOptions"].end(); itr++)
		{
			Json::Value option = (*itr);
			serverOptions.push_back(option.asString());
		}
		std::string serverType = server["serverType"].asString();
		if (serverType == "vanilla") {
			std::string backupPath = server["backupPath"].asString();
			std::vector<std::string> worldsToBackup;
			for (Json::Value::iterator itr = server["worldsToBackup"].begin(); itr != server["worldsToBackup"].end(); itr++)
			{
				Json::Value world = (*itr);
				worldsToBackup.push_back(world.asString());
			}
			servers->push_back(new MinecraftServerService::VanillaServer(serverName ,serverPath, serverJarName, serverAccount, maxHeapAlloc, minHeapAlloc, gcThreadCount, backupPath, worldsToBackup, javaArgs, serverOptions));
			servers->back()->startServer();
		} else if (serverType == "spigot") {
			std::string backupPath = server["backupPath"].asString();
			std::vector<std::string> worldsToBackup;
			for (Json::Value::iterator itr = server["worldsToBackup"].begin(); itr != server["worldsToBackup"].end(); itr++)
			{
				Json::Value world = (*itr);
				worldsToBackup.push_back(world.asString());
			}
			servers->push_back(new MinecraftServerService::SpigotServer(serverName ,serverPath, serverJarName, serverAccount, maxHeapAlloc, minHeapAlloc, gcThreadCount, backupPath, worldsToBackup, javaArgs, serverOptions));
			servers->back()->startServer();
		} else if (serverType == "bukkit") {
			std::string backupPath = server["backupPath"].asString();
			std::vector<std::string> worldsToBackup;
			for (Json::Value::iterator itr = server["worldsToBackup"].begin(); itr != server["worldsToBackup"].end(); itr++)
			{
				Json::Value world = (*itr);
				worldsToBackup.push_back(world.asString());
			}
			servers->push_back(new MinecraftServerService::BukkitServer(serverName ,serverPath, serverJarName, serverAccount, maxHeapAlloc, minHeapAlloc, gcThreadCount, backupPath, worldsToBackup, javaArgs, serverOptions));
			servers->back()->startServer();
		} else if (serverType == "bungeecord") {
			std::string backupPath = server["backupPath"].asString();
			servers->push_back(new MinecraftServerService::BungeeCordServer(serverName ,serverPath, serverJarName, serverAccount, maxHeapAlloc, minHeapAlloc, gcThreadCount, backupPath, javaArgs, serverOptions));
			servers->back()->startServer();
		} else if (serverType == "other") {
			std::string backupPath = server["backupPath"].asString();
			std::vector<std::string> worldsToBackup;
			for (Json::Value::iterator itr = server["worldsToBackup"].begin(); itr != server["worldsToBackup"].end(); itr++)
			{
				Json::Value world = (*itr);
				worldsToBackup.push_back(world.asString());
			}
			servers->push_back(new MinecraftServerService::GenericServer(serverName ,serverPath, serverJarName, serverAccount, maxHeapAlloc, minHeapAlloc, gcThreadCount, backupPath, worldsToBackup, javaArgs, serverOptions));
			servers->back()->startServer();
		} else {
			log.fatal("No such server type "+serverType);
			exit(-1);
		}
	}
	return servers;
}
