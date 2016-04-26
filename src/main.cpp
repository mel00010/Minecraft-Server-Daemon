#include "server.h"
#include "log.h"
#include "parser.h"
#include <json/json.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#define PIPE_READ 0
#define PIPE_WRITE 1
Logger logger;

int main(void) {
	/* Our process ID and Session ID */
	//~ pid_t pid, sid;

	/* Fork off the parent process */
	//~ pid = fork();
	//~ if (pid < 0) {
		//~ exit(EXIT_FAILURE);
	//~ }
	/* If we got a good PID, then
	   we can exit the parent process. */
	//~ if (pid > 0) {
		//~ exit(EXIT_SUCCESS);
	//~ }
	/* Change the file mode mask */
	//~ umask(0);
			
	/* Open any logs here */        
	Parser parser;
	Json::Value config = parser.parse("/etc/minecraft/config.json");
	logger = Logger(config["options"]["logDir"].asString(),config["options"]["logDir"].asString());
	/* Create a new SID for the child process */
	//~ sid = setsid();
	//~ if (sid < 0) {
		//~ /* Log the failure */
		//~ exit(EXIT_FAILURE);
	//~ }
	

	
	/* Change the current working directory */
	if ((chdir("/")) < 0) {
		/* Log the failure */
		//~ exit(EXIT_FAILURE);
	}
	
	/* Close out the standard file descriptors */
	//~ close(STDIN_FILENO);
	//~ close(STDOUT_FILENO);
	//~ close(STDERR_FILENO);
	
	/* Daemon-specific initialization goes here */
	std::vector<Server> servers;
	for (Json::Value::iterator itr = config["servers"].begin(); itr != config["servers"].end(); itr++)
	{
		
		Json::Value server = (*itr);
		std::string serverName = server["serverName"].asString();
		logger.logToFile(serverName, "NOTICE");
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
		logger.logToFile("Read server config", "INFO");
		servers.push_back(Server(serverName ,serverPath, serverJarName, serverAccount, maxHeapAlloc, minHeapAlloc, gcThreadCount, backupPath, worldsToBackup, javaArgs, serverOptions));
		logger.logToFile("Constructed server", "INFO");
		servers.back().startServer();
		//~ sleep(10);
		//~ servers.back().stopServer();
	}
	char buf;
	//~ sleep();
	/* The Big Loop */
	while (1) {
	   /* Do some task here ... */
	    read(servers.back().connection.serverstdout[PIPE_READ], &buf, 1);
		std::cout << buf;
		//~ logger.logToFile("Big loop", "INFO");
		//~ sleep(30); /* wait 30 seconds */
	}
   //~ exit(EXIT_SUCCESS);
}
