#include "connection.h"
#include "server.h"
#include "log4cpp/Category.hh"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <thread>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include <pstreams/pstream.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <memory>

//~ #define PIPE_READ 0
//~ #define PIPE_WRITE 1
//~ #define MAX_PIPE_SIZE 16384

Connection::Connection(log4cpp::Category& log) : log{log}
{
	//~ log = _log;
	log.info("Constructed connection object");
	//~ startServer();
}
Connection::~Connection()
{
	//~ server->close();
}
std::shared_ptr<redi::pstream> Connection::startServer(
	std::string serverPath, std::string serverJarName, std::string serverAccount,
	int maxHeapAlloc, int minHeapAlloc, int gcThreadCount,
	std::vector<std::string> javaArgs, std::vector<std::string> serverOptions
)
{
	log.info("Starting " + serverJarName);
	std::vector<std::string> serverCommand;
	serverCommand.push_back("/usr/bin/java");
	serverCommand.push_back("-Xmx"+std::to_string(maxHeapAlloc)+"M");
	serverCommand.push_back("-Xms"+std::to_string(minHeapAlloc)+"M");
	serverCommand.push_back("-XX:ParallelGCThreads="+std::to_string(gcThreadCount));
	for (int i = 0; i < javaArgs.size(); ++i)
	{
		serverCommand.push_back("-"+javaArgs[i]);
	}
	serverCommand.push_back("-jar");
	serverCommand.push_back(serverPath+"/"+serverJarName);
	//~ std::cout << "generated command" << std::endl;
	for (int i = 0; i < serverOptions.size(); ++i)
	{
		serverCommand.push_back(serverOptions[i]);
	}
	//~ char** javaCStringArgs = new char*[serverCommand.size()+2];
	//~ for(int i = 0; i < serverCommand.size(); ++i)
	//~ {
		//~ log.info(serverCommand[i]);
		//~ javaCStringArgs[i] = new char[serverCommand[i].size() + 1];
		//~ strcpy(javaCStringArgs[i], serverCommand[i].c_str());
	//~ }
	//~ javaCStringArgs[serverCommand.size()]= nullptr;
	chdir(serverPath.c_str());
	log.info("Generated complete command");
	server = std::make_shared<redi::pstream>("/usr/bin/java", serverCommand, redi::pstreambuf::pstdout|redi::pstreambuf::pstderr|redi::pstreambuf::pstdin);
	//~ server = &_server;
	log.info("Launched server process");
	//~ server = _server;
	return server;
}
void Connection::stopServer(/*std::shared_ptr<redi::pstream> server*/)
{
	//~ log.info("Stopping "+serverJarName);
	sendCommand("say SERVER SHUTTING DOWN IN 10 SECONDS.");
	for (int i=10; i>0; --i) {
		sendCommand("say "+std::to_string(i));
		sleep(1);
	}
	sendCommand("say Saving map...");
	sendCommand("save-all");
	sendCommand("stop");
	log.info("Server stopped");
	//~ stopped.store(true);
}
void Connection::sendCommand(std::string command)
{
	log.info(command);
	command = command;
	*server << command <<std::endl;
}



