#include "connection.h"
#include "log.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <thread>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include <atomic>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

#define PIPE_READ 0
#define PIPE_WRITE 1
//~ #define MAX_PIPE_SIZE 16384

Connection::Connection()
{
	logger.logToFile("Constructed connection object", "INFO");
	//~ startServer();
}
Connection::~Connection()
{
	close(serverstdin[PIPE_WRITE]);
	close(serverstdout[PIPE_READ]);
}
int Connection::startServer(
	std::string serverPath, std::string serverJarName, std::string serverAccount,
	int maxHeapAlloc, int minHeapAlloc, int gcThreadCount,
	std::vector<std::string> javaArgs, std::vector<std::string> serverOptions
)
{
	logger.logToFile("Starting " + serverJarName, "NOTICE");
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
	char** javaCStringArgs = new char*[serverCommand.size()+2];
	for(int i = 0; i < serverCommand.size(); ++i)
	{
		//~ logger.logToFile(serverCommand[i], "INFO");
		javaCStringArgs[i] = new char[serverCommand[i].size() + 1];
		strcpy(javaCStringArgs[i], serverCommand[i].c_str());
	}
	javaCStringArgs[serverCommand.size()]= nullptr;
	if (pipe(serverstdin) < 0) {
		logger.logToFile("Error while allocating pipe for child input redirect", "SEVERE");
		//~ exit(-1);
	}
	if (pipe(serverstdout) < 0) {
		close(serverstdin[PIPE_READ]);
		close(serverstdin[PIPE_WRITE]);
		logger.logToFile("Error while allocating pipe for child output redirect", "SEVERE");
		//~ exit(-1);
	}
	server = fork();
	
	if (0 == server) {
		// child continues here
		//~ std::cerr<< "child" << std::endl;
		// redirect stdin
		//~ if (dup2(serverstdin[PIPE_READ], STDIN_FILENO) == -1) {
			//~ logger.logToFile("Error occurred while redirecting stdin", "SEVERE");
			//~ exit(-1);
		//~ }
		//~ std::cerr<< "redirected stdin"<< std::endl;
		
		// redirect stdout
		if (dup2(serverstdout[PIPE_WRITE], STDOUT_FILENO) == -1) {
			logger.logToFile("Error occurred while redirecting stdout", "SEVERE");
			//~ exit(-1);
		}
		//~ std::cerr<< "redirected stdout" << std::endl;
		// redirect stderr
		if (dup2(serverstdout[PIPE_WRITE], STDERR_FILENO) == -1) {
			logger.logToFile("Error occurred while redirecting stderr", "SEVERE");
			//~ exit(-1);
		}
		//~ std::cout<< "redirected stderr" << std::endl;
		// all these are for use by parent only
		close(serverstdin[PIPE_READ]);
		close(serverstdin[PIPE_WRITE]);
		close(serverstdout[PIPE_READ]);
		close(serverstdout[PIPE_WRITE]); 
		chdir(serverPath.c_str());
		// run child process image
		// replace this with any exec* function find easier to use ("man exec")
		//~ std::cerr <<"about to exec the child process"<< std::endl;
		int exitCode = execv("/usr/bin/java", javaCStringArgs);
		//~ std::cerr <<"error executing the child process"<< std::endl;
		// if we get here at all, an error occurred, but we are in the child
		// process, so just exit
		logger.logToFile("Child process exec failed", "SEVERE");
		//~ exit(exitCode);
	} else if (server > 0) {
		// parent continues here
		// close unused file descriptors, these are for child only
		close(serverstdin[PIPE_READ]);
		close(serverstdout[PIPE_WRITE]); 
		// done with these in this example program, you would normally keep these
		// open of course as long as you want to talk to the child
		logger.logToFile(serverJarName + " started", "NOTICE");
		return server;
	} else {
		// failed to create child
		logger.logToFile("Failed to fork", "SEVERE");
		close(serverstdin[PIPE_READ]);
		close(serverstdin[PIPE_WRITE]);
		close(serverstdout[PIPE_READ]);
		close(serverstdout[PIPE_WRITE]);
	}
}
void Connection::stopServer()
{
	//~ logger.logToFile("Stopping "+serverJarName, "NOTICE");
	sendCommand("say SERVER SHUTTING DOWN IN 10 SECONDS.");
	for (int i=10; i>0; --i) {
		sendCommand("say "+std::to_string(i));
		sleep(1);
	}
	sendCommand("say Saving map...");
	sendCommand("save-all");
	sendCommand("stop");
	logger.logToFile("Server stopped", "NOTICE");
	//~ stopped.store(true);
}
void Connection::sendCommand(std::string command)
{
	logger.logToFile(command, "INFO");
	command = command + "\n";
	//~ std::stringstream output;
	write(serverstdin[PIPE_WRITE], command.c_str(), strlen(command.c_str()));
	//~ char buf
	//~ ssize_t nbytes;
	//~ while ((nbytes=read(serverstdout[PIPE_READ], &buf, 1)) > 0) {
		//~ output << buf;
	//~ }
	//~ while (read(serverstdout[PIPE_READ], &buf, 1) == 1) {
		//~ output << buf;
	//~ }
	//~ return output;
}
void Connection::listenForOutput()
{
	//~ while(true)
	//~ {
		char buf;
		std::stringstream line;
		do 
		{
			read(serverstdout[PIPE_READ], &buf, 1);
			line << buf;	
		} while (buf != '\n');
		logger.logToFile(line.str(), "INFO");
	//~ }
}

