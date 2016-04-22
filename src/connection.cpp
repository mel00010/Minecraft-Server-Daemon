#include "connection.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <thread>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include <unistd.h>
#include <string.h>

#define PIPE_READ 0
#define PIPE_WRITE 1
//~ #define MAX_PIPE_SIZE 16384

Connection::Connection(
	std::string _serverPath, std::string _serverJarName, std::string _serverAccount,
	int _maxHeapAlloc, int _minHeapAlloc, int _gcThreadCount, int _historyLength, std::string _backupPath,
	std::vector<std::string> _worldsToBackup,std::vector<std::string> _javaArgs, std::vector<std::string> _serverOptions
)
{
	serverPath = _serverPath;
	serverJarName = _serverJarName;
	serverAccount = _serverAccount;
	maxHeapAlloc = _maxHeapAlloc;
	minHeapAlloc = _minHeapAlloc;
	gcThreadCount = _gcThreadCount;
	backupPath = _backupPath;
	worldsToBackup = std::vector<std::string>(_worldsToBackup);
	javaArgs = std::vector<std::string>(_javaArgs);
	serverOptions = std::vector<std::string>(_serverOptions);
	//~ startServer();
}
Connection::~Connection()
{
	close(serverstdin[PIPE_WRITE]);
	close(serverstdout[PIPE_READ]);
}
int Connection::startServer()
{
	std::cout << "Starting " << serverJarName << std::endl;
	std::vector<std::string> serverCommand;
	std::cout << "Created command vector" << std::endl;
	serverCommand.push_back("/usr/bin/java");
	serverCommand.push_back("-Xmx"+std::to_string(maxHeapAlloc)+"M");
	serverCommand.push_back("-Xms"+std::to_string(minHeapAlloc)+"M");
	serverCommand.push_back("-XX:ParallelGCThreads="+std::to_string(gcThreadCount));
	std::cout << "Added memory and gc args to command vector" << std::endl;
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
	std::cout << "generated command" << std::endl;
	char** javaCStringArgs = new char*[serverCommand.size()+2];
	for(int i = 0; i < serverCommand.size(); ++i)
	{
		std::cout << serverCommand[i] << std::endl;
		javaCStringArgs[i] = new char[serverCommand[i].size() + 1];
		strcpy(javaCStringArgs[i], serverCommand[i].c_str());
	}
	std::cout << "Converted to cstring" << std::endl;
	javaCStringArgs[serverCommand.size()]= nullptr;
	if (pipe(serverstdin) < 0) {
		std::cout<< "allocating pipe for child input redirect";
		exit(-1);
	}
	if (pipe(serverstdout) < 0) {
		close(serverstdin[PIPE_READ]);
		close(serverstdin[PIPE_WRITE]);
		std::cout<<"allocating pipe for child output redirect";
		exit(-1);
	}
	std::cout<< "about to fork" << std::endl;
	server = fork();
	
	if (0 == server) {
		// child continues here
		//~ std::cerr<< "child" << std::endl;
		// redirect stdin
		if (dup2(serverstdin[PIPE_READ], STDIN_FILENO) == -1) {
			//~ std::cerr<< "error redirecting stdin";
			exit(-1);
		}
		//~ std::cerr<< "redirected stdin"<< std::endl;
		
		// redirect stdout
		if (dup2(serverstdout[PIPE_WRITE], STDOUT_FILENO) == -1) {
			//~ std::cerr<<"error redirecting stdout";
			exit(-1);
		}
		//~ std::cerr<< "redirected stdout" << std::endl;
		// redirect stderr
		if (dup2(serverstdout[PIPE_WRITE], STDERR_FILENO) == -1) {
			//~ std::cout<<"error redirecting stderr";
			exit(-1);
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
		exit(exitCode);
	} else if (server > 0) {
		// parent continues here
		std::cout<< "server" << std::endl;
		// close unused file descriptors, these are for child only
		close(serverstdin[PIPE_READ]);
		close(serverstdout[PIPE_WRITE]); 
		// done with these in this example program, you would normally keep these
		// open of course as long as you want to talk to the child
		std::cout << "Server started." << std::endl;
		return server;
	} else {
		// failed to create child
		std::cout << "Failed to fork" << std::endl;
		close(serverstdin[PIPE_READ]);
		close(serverstdin[PIPE_WRITE]);
		close(serverstdout[PIPE_READ]);
		close(serverstdout[PIPE_WRITE]);
	}
}
void Connection::stopServer()
{
	sendCommand("say SERVER SHUTTING DOWN IN 10 SECONDS.");
	for (int i=10; i>0; --i) {
		sendCommand("say "+std::to_string(i));
		sleep(1);
	}
	sendCommand("say Saving map...");
	sendCommand("save-all");
	sendCommand("stop");
	std::cout << "Server " << serverJarName << " stopped" << std::endl;
}
void Connection::listOnlinePlayers()
{
	
	std::stringstream output;
	std::string line;
	int numPlayers = 0;
	
	std::stringstream playerList = sendCommand("list");
	std::stringbuf *listBuf = playerList.rdbuf();
	
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	while (listBuf->in_avail() != 0)
	{
		getline(playerList, line);
		output << line << std::endl;
		numPlayers++;
	}
	if (numPlayers != 0) {
		std::string outputBuf = output.str();
		std::cout << outputBuf << std::endl;
	} else {
		std::cout << "No one is on the server" << std::endl;
	}
}
void Connection::listOnlinePlayers(std::string playerName)
{
	std::stringstream output;
	std::string line;
	int numPlayers = 0;
	
	std::stringstream playerList = sendCommand("list");
	std::stringbuf *listBuf = playerList.rdbuf();
	
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	while (listBuf->in_avail() != 0)
	{
		getline(playerList, line);
		if (line != playerName) {
			output << line << std::endl;
			numPlayers++;
		}
	}
	if (numPlayers != 0) {
		std::string outputBuf = output.str();
		std::cout << outputBuf << std::endl;
	} else {
		std::cout << "No one is on the server" << std::endl;
	}
}
std::stringstream Connection::sendCommand(std::string command)
{
	command = command + "\n";
	std::stringstream output;
	write(serverstdin[PIPE_WRITE], command.c_str(), strlen(command.c_str()));
	//~ char buf
	//~ ssize_t nbytes;
	//~ while ((nbytes=read(serverstdout[PIPE_READ], &buf, 1)) > 0) {
		//~ output << buf;
	//~ }
	//~ while (read(serverstdout[PIPE_READ], &buf, 1) == 1) {
		//~ output << buf;
	//~ }
	return output;
}

