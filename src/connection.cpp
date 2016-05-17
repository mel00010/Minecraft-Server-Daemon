#include "connection.h"
#include "server.h"
#include "log4cpp/Category.hh"
#include "log4cpp/Appender.hh"
#include "log4cpp/FileAppender.hh"
#include "log4cpp/Layout.hh"
#include "log4cpp/PatternLayout.hh"
#include "log4cpp/Priority.hh"
#include <string>
#include <vector>
#include <sstream>
#include <thread>
#include <time.h>
#include <pstreams/pstream.h>
#include <event2/event.h>

Connection::Connection(std::string _serverName, struct event_base *_event_base)
{
	serverName = _serverName;
	event_base =_event_base;
	log4cpp::Appender *appender = new log4cpp::FileAppender(serverName, "/var/log/minecraft/servers/"+serverName+".log");
	log4cpp::PatternLayout* pattern = new log4cpp::PatternLayout();
	pattern->setConversionPattern("%d %c [%p] %m%n");
	appender->setLayout(pattern);
	log = &log4cpp::Category::getInstance(serverName);
	log->addAppender(appender);
	log->debug("Connection::Connection");
}
Connection::~Connection()
{
	log->debug("Connection::~Connection");
	log->getAppender()->close();
	//~ delete(log);
}
void Connection::startServer(
	std::string serverPath, std::string serverJarName, std::string serverAccount,
	int maxHeapAlloc, int minHeapAlloc, int gcThreadCount,
	std::vector<std::string> javaArgs, std::vector<std::string> serverOptions
)
{
	log->debug("Connection::startServer");
	if(running)
	{
		if(server->rdbuf()->exited())
		{
			log->info("Starting " + serverJarName);
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
			for (int i = 0; i < serverOptions.size(); ++i)
			{
				serverCommand.push_back(serverOptions[i]);
			}
			
			chdir(serverPath.c_str());
			log->debug("Generated complete command");
			server.reset();
			server = std::make_shared<redi::pstream>("/usr/bin/java", serverCommand, redi::pstreambuf::pstdout|redi::pstreambuf::pstderr|redi::pstreambuf::pstdin);
			log->debug("Launched server process");
			running = true;
			std::thread outputListener(&Connection::outputListener, this);
			outputListener.detach();
		} else {
			log->info("Server already running");
		}
	} else if (!running) {
		log->info("Starting " + serverJarName);
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
		for (int i = 0; i < serverOptions.size(); ++i)
		{
			serverCommand.push_back(serverOptions[i]);
		}
		
		chdir(serverPath.c_str());
		log->debug("Generated complete command");
		server.reset();
		server = std::make_shared<redi::pstream>("/usr/bin/java", serverCommand, redi::pstreambuf::pstdout|redi::pstreambuf::pstderr|redi::pstreambuf::pstdin);
		log->debug("Launched server process");
		running = true;
		std::thread outputListener(&Connection::outputListener, this);
		outputListener.detach();
	}
}
void Connection::stopServer()
{
	log->debug("Connection::stopServer");
	if(running)
	{
		log->notice("Server shutting down");
		sendCommand("say SERVER SHUTTING DOWN IN 10 SECONDS.");
		for (int i=10; i>0; --i) {
			sendCommand("say "+std::to_string(i));
			sleep(1);
		}
		sendCommand("say Saving map...");
		sendCommand("save-all");
		sendCommand("stop");
		while(!server->rdbuf()->exited()) {}
		running = false;
		log->info("Server stopped");
	} else {
		log->info("Server not running");
	}
	
}
void Connection::sendCommand(std::string command)
{
	log->debug("Connection::sendCommand");
	if(running)
	{
		log->info("Sending command \"" + command + "\" to "+ serverName);
		command = command;
		*server << command <<std::endl;
	} else {
		log->info("Server not running");
	}
}
void Connection::outputListener()
{
	log->debug("Connection::outputListener");
	std::weak_ptr<redi::pstream> _server = server;
	//~ event_reinit(event_base);
	std::string output;
	auto __server = _server.lock();
	while (running) {
		std::getline(*__server, output);
		if(output.size() > 1)
		{
			log->info(output);
		}
	}
	__server.reset();
}


