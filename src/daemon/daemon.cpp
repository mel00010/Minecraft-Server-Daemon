/*******************************************************************************
 *
 * Minecraft Server Daemon
 * daemon.cpp
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

#include <CreateSocket.hpp>
#include <event2/event.h>
#include <json/json.h>
#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <MainLoop.hpp>
#include <ConfigFileParser.hpp>
#include <Server.hpp>
#include <signal.h>
#include <sys/stat.h>
#include <SetupServers.hpp>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <vector>

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
	ConfigFileParser parser;
	Json::Value config = parser.parseConfigFile("/etc/minecraft/config.json");
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
	int controlSocket = CreateSocket(rootLog);
	std::vector<MinecraftServerDaemon::Server*>* servers;
	servers = setupServers(&config, rootLog);
	/* The Big Loop */
	mainLoop(servers, rootLog, controlSocket, base);
	
	exit(EXIT_SUCCESS);
}
