#include "server.h"
#include "parser.h"
#include "mainLoop.h"
#include "createSocket.h"
#include "setupServers.h"
#include <unistd.h>
#include <signal.h>
#include <unistd.h>
#include <json/json.h>
#include "log4cpp/Category.hh"
#include <log4cpp/PropertyConfigurator.hh>
#if DEBUGGING == 1
	#include <iostream>
#endif /* DEBUGGING == 1 */
#include <event2/event.h>
int main(void) {
	#if DEBUGGING == 0
	/* Our process ID and Session ID */
	pid_t pid, sid;

	/* Fork off the parent process */
	pid = fork();
	if (pid < 0) {
		std::cout << "Failure forking from parent process" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (pid > 0) {
		exit(EXIT_SUCCESS);
	}
	pid = fork();
	if (pid < 0) {
		std::cout << "Failure forking from parent process" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (pid > 0) {
		exit(EXIT_SUCCESS);
	}
	/* If we got a good PID, then
	   we can exit the parent process. */
	#endif	/* DEBUGGING == 0 */
	signal(SIGINT, sigint_handler);
	/* Change the file mode mask */
	umask(0);
	/* Open any logs here */        
	//Logging
	Parser parser;
	Json::Value config = parser.parse("/etc/minecraft/config.json");
	//~ std::string initFileName = "/etc/minecraft/log4cpp.properties";
	log4cpp::PropertyConfigurator::configure("/etc/minecraft/log4cpp.properties");

	log4cpp::Category& rootLog = log4cpp::Category::getRoot();
	
	/* Create a new SID for the child process */
	#if DEBUGGING == 0
	sid = setsid();
	if (sid < 0) {
		//~ /* Log the failure */
		rootLog.fatal("Failure creating new sessionID for daemon");
		exit(EXIT_FAILURE);
	}
	#endif /* DEBUGGING == 0 */
	/* Change the current working directory */
	if ((chdir("/")) < 0) {
		/* Log the failure */
		rootLog.fatal("Failure changing working directory");
		exit(EXIT_FAILURE);
	}
	#if DEBUGGING == 0
	/* Close out the standard file descriptors */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	#endif /* DEBUGGING == 0 */
	/* Daemon-specific initialization goes here */
	//Create pipe to recieve commands from control program
	
	// Read from config file and set up servers
	struct event_base* base = event_base_new();
	int controlSocket = createSocket(rootLog);
	std::vector<MinecraftServerService::Server*>* servers;
	servers = setupServers(&config, rootLog);
	/* The Big Loop */
	mainLoop(servers, rootLog, controlSocket, base);
	
	exit(EXIT_SUCCESS);
}
