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

#include <config.h>
#include <ConfigFileParser.hpp>
#include <event2/event.h>
#include <IPC.hpp>
#include <json/json.h>
#include <log4cpp/Category.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Priority.hh>
#include <MainLoop.hpp>
#include <signal.h>
#include <sys/stat.h>
#include <Server.hpp>
#include <SetupServers.hpp>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <vector>

/**
 * Main namespace for all daemon related functions, classes, structs, enums, and variables.
 */
namespace MinecraftServerDaemon {
}
/**
 * The main function.
 * @return
 */
int main(void) {
#ifndef __DEBUGGING__
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
#endif	/* __DEBUGGING__ */
	signal(SIGINT, sigint_handler);
	/* Change the file mode mask */
	umask(0);
	/* Open any logs here */
	//Logging
	ConfigFileParser parser;
	Json::Value config = parser.parseConfigFile(__CONFIG_FILE_NAME__);

	log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
	layout->setConversionPattern("%d %c [%p] %m%n ");
	log4cpp::Appender *consoleAppender = new log4cpp::OstreamAppender("console", &std::cout);
	consoleAppender->setLayout(layout);
	log4cpp::Appender *daemonFileAppender = new log4cpp::FileAppender("daemon", std::string(__LOG_DIR__) + "/daemon.log");
	daemonFileAppender->setLayout(layout);
	log4cpp::Category& rootLog = log4cpp::Category::getRoot();
	rootLog.setPriority(log4cpp::Priority::DEBUG);
	rootLog.addAppender(consoleAppender);

	log4cpp::Category& daemonLog = log4cpp::Category::getInstance(std::string("daemon"));
	daemonLog.addAppender(daemonFileAppender);

	/* Create a new SID for the child process */
#ifndef __DEBUGGING__
	sid = setsid();
	if (sid < 0) {
		/* Log the failure */
		daemonLog.fatal("Failure creating new sessionID for daemon");
		exit(EXIT_FAILURE);
	}
#endif /* __DEBUGGING__ */
	/* Change the current working directory */
	if ((chdir("/")) < 0) {
		/* Log the failure */
		daemonLog.fatal("Failure changing working directory");
		exit(EXIT_FAILURE);
	}
#ifndef __DEBUGGING__
	/* Close out the standard file descriptors */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
#endif /* __DEBUGGING__ */
	// Create socket to recieve commands from control program
	// Read from config file and set up servers
	struct event_base* base = event_base_new();
	int controlSocket = createSocket(daemonLog);
	std::vector<MinecraftServerDaemon::Server*>* servers;
	servers = setupServers(&config, daemonLog);
	/* The Big Loop */
	mainLoop(servers, daemonLog, controlSocket, base);

	exit(EXIT_SUCCESS);
}
