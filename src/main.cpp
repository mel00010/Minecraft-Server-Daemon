#include "server.h"
#include "parser.h"
#include <json/json.h>
#include "log4cpp/Category.hh"
#include <log4cpp/PropertyConfigurator.hh>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
//~ std::string readFromPipe()
//~ {
	//~ char buf[4];
	//~ rc=read(fd[0],buf,4);
	//~ int size = atoi(buf);
	//~ root.debug(std::to_string(size));
	//~ char line[size];
	//~ rc = read(fd[0],line,size);
	//~ line[rc] = '\0';
	//~ std::string buff(line);
	//~ return line;
//~ }
//~ void writeToPipe(std::string message)
//~ {
	//~ std::ostringstream ss;
	//~ ss << std::setw(4) << std::setfill('0') << message.size();
	//~ std::string result = ss.str();
	//~ std::string buf = ss.str() + message;
	//~ write(fd[1], buf.c_str(), buf.size());
//~ }
int main(void) {
	/* Our process ID and Session ID */
	pid_t pid, sid;

	/* Fork off the parent process */
	//~ pid = fork();
	//~ if (pid < 0) {
		//~ root.fatal("Failure forking from parent process");
		//~ exit(EXIT_FAILURE);
	//~ }
	//~ if (pid > 0) {
		//~ exit(EXIT_SUCCESS);
	//~ }
	/* If we got a good PID, then
	   we can exit the parent process. */
	/* Change the file mode mask */
	umask(0);
			
	/* Open any logs here */        
	//Logging
	Parser parser;
	Json::Value config = parser.parse("/etc/minecraft/config.json");
    std::string initFileName = "/etc/minecraft/log4cpp.properties";
	log4cpp::PropertyConfigurator::configure(initFileName);

	log4cpp::Category& root = log4cpp::Category::getRoot();

	log4cpp::Category& serverLog = 
		log4cpp::Category::getInstance(std::string("server"));
	
	/* Create a new SID for the child process */
	sid = setsid();
	if (sid < 0) {
		//~ /* Log the failure */
		root.fatal("Failure creating new sessionID for daemon");
		exit(EXIT_FAILURE);
	}
	

	
	/* Change the current working directory */
	if ((chdir("/")) < 0) {
		/* Log the failure */
		root.fatal("Failure changing working directory");
		exit(EXIT_FAILURE);
	}
	
	/* Close out the standard file descriptors */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	
	/* Daemon-specific initialization goes here */
	
	//Create listen socket to recieve commands from control program
	char *controlPipePath = "/etc/minecraft/control.pipe";
	//~ char *outputPipePath = "/etc/minecraft/output.pipe";
	mkfifo(controlPipePath, 0666);
	//~ mkfifo(outputPipePath, 0666);
	
	// Read from config file and set up servers
	std::vector<Server> servers;
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
		servers.push_back(Server(serverName ,serverPath, serverJarName, serverAccount, maxHeapAlloc, minHeapAlloc, gcThreadCount, backupPath, worldsToBackup, javaArgs, serverOptions, serverLog));
		servers.back().startServer();
	}
	root.info("Opening pipes");
	char buf[4];
	int fd,rc;
	fd = open(controlPipePath, O_RDONLY);
	//~ fd[1] = open(outputPipePath, O_WRONLY);
	root.info("Opened pipes");
	/* The Big Loop */
	while (true) {
		while ( (rc=read(fd,buf,4)) > 0) {
			buf[rc] = '\0';
			int size = atoi(buf);
			root.debug(std::to_string(size));
			char line[size];
			rc = read(fd,line,size);
			line[rc] = '\0';
			root.info("Client connected to control pipe");
			std::string command(line);
			root.debug(command);
			if (command == "startServer") {
				root.debug("Command recieved:  startServer");
				rc=read(fd,buf,4);
				size = atoi(buf);
				root.debug(std::to_string(size));
				char line[size];
				rc = read(fd,line,size);
				line[rc] = '\0';
				std::string serverRequested(line);
				root.debug("Server name recieved");
				for( Server i : servers) {
					root.info(i.serverName);
					if (i.serverName == serverRequested) {
						root.debug("Server starting not implemented yet");
						break;
					}
				}
			} else if (command == "stopServer") {
				root.debug("Command recieved:  stopServer");
				rc=read(fd,buf,4);
				size = atoi(buf);
				root.debug(std::to_string(size));
				char line[size];
				rc = read(fd,line,size);
				line[rc] = '\0';
				std::string serverRequested(line);
				root.debug("Server name recieved");
				for( Server i : servers) {
					root.info(i.serverName);
					if (i.serverName == serverRequested) {
						root.debug("Found server");
						i.stopServer();
						break;	
					}
				}
			} else if (command == "stopAll") {
				root.debug("Command recieved:  stopAll");
				for( Server i : servers) {
					i.stopServer();
				}
			} else if (command == "restartServer") {
				root.debug("Command recieved:  restartServer");
				rc=read(fd,buf,4);
				size = atoi(buf);
				root.debug(std::to_string(size));
				char line[size];
				rc = read(fd,line,size);
				line[rc] = '\0';
				std::string serverRequested(line);
				root.debug("Server name recieved");
				for( Server i : servers) {
					root.info(i.serverName);
					if (i.serverName == serverRequested) {
						i.restartServer();
						break;
					}
				}
			} else if (command == "restartAll") {
				root.debug("Command recieved:  restartAll");
				for( Server i : servers) {
					i.restartServer();
				}
			} else if (command == "serverStatus") {
				root.debug("Command recieved:  serverStatus");
				size = atoi(buf);
				root.debug(std::to_string(size));
				char line[size];
				rc = read(fd,line,size);
				line[rc] = '\0';
				std::string serverRequested(line);
				root.debug("Server name recieved");
				for( Server i : servers) {
					root.info(i.serverName);
					if (i.serverName == serverRequested) {
						root.warn("Status not implemented yet");
						break;
					}
				}
			} else if (command == "listServers") {
				root.debug("Command recieved:  listServers");
				for( Server i : servers) {
					root.debug(i.serverName);
				}
			} else if (command == "sendCommand") {
				root.debug("Command recieved:  sendCommand");
				rc=read(fd,buf,4);
				size = atoi(buf);
				root.debug(std::to_string(size));
				char line[size];
				rc = read(fd,line,size);
				line[rc] = '\0';
				std::string serverRequested(line);
				root.debug(serverRequested);
				root.debug("Server name recieved");
				for( Server i : servers) {
					root.info(i.serverName);
					if (i.serverName == serverRequested) {
						root.debug("Found server");
						rc=read(fd,buf,4);
						size = atoi(buf);
						root.debug(std::to_string(size));
						char buff[size];
						rc = read(fd,buff,size);
						buff[rc] = '\0';
						std::string serverCommand(buff);
						root.debug("Server command recieved");
						root.debug(serverCommand);
						i.sendCommand(serverCommand);
						break;
					}
				}
			} else if (command == "listOnlinePlayers") {
				root.debug("Command recieved:  listOnlinePlayers");
				rc=read(fd,buf,4);
				size = atoi(buf);
				root.debug(std::to_string(size));
				char line[size];
				rc = read(fd,line,size);
				line[rc] = '\0';
				std::string serverRequested(line);
				root.debug("Server name recieved");
				for( Server i : servers) {
					root.info(i.serverName);
					if (i.serverName == serverRequested) {
						i.listOnlinePlayers();
						break;
					}
				}
			} else if (command == "listOnlinePlayersFiltered") {
				root.info("Command recieved:  listOnlinePlayersFiltered");
				rc=read(fd,buf,4);
				size = atoi(buf);
				root.debug(std::to_string(size));
				char line[size];
				rc = read(fd,line,size);
				line[rc] = '\0';
				std::string serverRequested(line);
				root.info("Server name recieved");
				for( Server i : servers) {
					root.info(i.serverName);
					if (i.serverName == serverRequested) {
						rc=read(fd,buf,4);
						size = atoi(buf);
						root.debug(std::to_string(size));
						char buff[size];
						rc = read(fd,buff,size);
						buff[rc] = '\0';
						std::string playerName(buff);
						root.debug("playerName recieved");
						root.debug(playerName);
						i.listOnlinePlayers(playerName);
						break;
					}
				}
			} else if (command == "updateServer") {
				root.debug("Command recieved:  updateServer");
				rc=read(fd,buf,4);
				size = atoi(buf);
				root.debug(std::to_string(size));
				char line[size];
				rc = read(fd,line,size);
				line[rc] = '\0';
				std::string serverRequested(line);
				root.debug("Server name recieved");
				for( Server i : servers) {
					root.info(i.serverName);
					if (i.serverName == serverRequested) {
						i.updateServer();
						break;
					}
				}
			} else if (command == "updateAll") {
				root.debug("Command recieved:  updateAll");
				for( Server i : servers) {
					i.updateServer();
				}
			} else if (command == "backupServer") {
				root.debug("Command recieved:  backupServer");
				rc=read(fd,buf,4);
				size = atoi(buf);
				root.debug(std::to_string(size));
				char line[size];
				rc = read(fd,line,size);
				line[rc] = '\0';
				std::string serverRequested(line);
				root.debug("Server name recieved");
				for( Server i : servers) {
					if (i.serverName == serverRequested) {
						i.backupServer();
						break;
					}
				}
			} else if (command == "backupAll") {
				root.debug("Command recieved:  backupAll");
				for( Server i : servers) {
					i.backupServer();
				}
			}
		}
		fd = open(controlPipePath, O_RDONLY);
		//~ fd[1] = open(outputPipePath, O_WRONLY);
		if (rc == -1) {
		  root.error("Read from control pipe failed");
		}
		else if (rc == 0) {
			root.info("Client disconnected from control pipe");
		}
		//~ sleep(5);
		
	}
	exit(EXIT_SUCCESS);
}
