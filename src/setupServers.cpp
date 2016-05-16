#include "setupServers.h"
#include "server.h"
#include <vector>
#include <json/json.h>
#include <string>
#include "log4cpp/Category.hh"
#include <log4cpp/PropertyConfigurator.hh>

std::vector<Server*>* setupServers(Json::Value* _config, struct event_base *base) {
	Json::Value config = *_config;
	std::vector<Server*>* servers = new std::vector<Server*>;
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
		std::string backupPath = server["backupPath"].asString();
		std::vector<std::string> worldsToBackup;
		std::vector<std::string> javaArgs;
		std::vector<std::string> serverOptions;
		for (Json::Value::iterator itr = server["worldsToBackup"].begin(); itr != server["worldsToBackup"].end(); itr++)
		{
			Json::Value world = (*itr);
			worldsToBackup.push_back(world.asString());
		}
		for (Json::Value::iterator itr = server["javaArgs"].begin(); itr != server["javaArgs"].end(); itr++)
		{
			Json::Value arg = (*itr);
			javaArgs.push_back(arg.asString());
		}
		for (Json::Value::iterator itr = server["serverOptions"].begin(); itr != server["serverOptions"].end(); itr++)
		{
			Json::Value option = (*itr);
			serverOptions.push_back(option.asString());
		}
		servers->push_back(new Server(serverName ,serverPath, serverJarName, serverAccount, maxHeapAlloc, minHeapAlloc, gcThreadCount, backupPath, worldsToBackup, javaArgs, serverOptions, base));
		servers->back()->startServer();
	}
	return servers;
}
