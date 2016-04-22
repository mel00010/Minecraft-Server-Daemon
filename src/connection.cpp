#include "connection.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <thread>
#include <chrono> 
#include <algorithm>
#include <iterator>


//~ Connection::Connection(std::string _screenSessionName)
//~ {
	//~ std::string line;
	//~ std::ifstream variables ("/tmp/"+_screenSessionName);
	//~ if (variables.is_open())
	//~ {
		//~ int i = 0;
		//~ while ( std::getline (variables,line) )
		//~ {
			//~ std::stringstream ssin(line);
			//~ switch(i)
			//~ {
				//~ case 0:
					//~ serverPath = line;
					//~ break;
				//~ case 1:
					//~ serverJarName = line;
					//~ break;
				//~ case 2:
					//~ serverAccount = line;
					//~ break;
				//~ case 3:
					//~ backupPath = line;
					//~ break;
				//~ case 4:
					//~ maxHeapAlloc = std::stoi(line);
					//~ break;
				//~ case 5:
					//~ minHeapAlloc = std::stoi(line);
					//~ break;
				//~ case 6:
					//~ gcThreadCount = std::stoi(line);
					//~ break;
				//~ case 7:
					//~ historyLength = std::stoi(line);
					//~ break;
				//~ case 8:
					//~ while (ssin.good()){
						//~ std::string word;
						//~ ssin >> word;
						//~ worldsToBackup.push_back(word);
					//~ }
					//~ break;
				//~ case 9:
					//~ while (ssin.good()){
						//~ std::string word;
						//~ ssin >> word;
						//~ javaArgs.push_back(word);
					//~ }					
					//~ break;
				//~ case 10:
					//~ while (ssin.good()) {
						//~ std::string word;
						//~ ssin >> word;
						//~ serverOptions.push_back(word);
					//~ }
					//~ break;
			//~ } 
			//~ i++;
		//~ }
		//~ variables.close();
		//~ screenSessionName = _screenSessionName;
	//~ }
//~ }
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
	startServer();
}
Connection::~Connection()
{
	close(aStdinPipe[PIPE_WRITE]);
	close(aStdoutPipe[PIPE_READ]);
}
void Connection::startServer()
{
	std::cout << "Starting " << serverJarName << std::endl;
	std::string  = "java -Xmx"+std::to_string(maxHeapAlloc)+"M";
	invocation +=" -Xms" +  std::to_string(minHeapAlloc)+"M -XX:ParallelGCThreads="+std::to_string(gcThreadCount);
	for(std::string i : javaArgs) {
		invocation += " -" + i;
	}
	invocation += " -jar " + serverPath+"/"+serverJarName;
	//~ std::cout << invocation << std::endl;
	if (pipe(stdin) < 0) {
		exit(-1);
	}
	if (pipe(stdout) < 0) {
		close(stdin[PIPE_READ]);
		close(stdin[PIPE_WRITE]);
		exit(-1);
	}

	server = fork();
	if (0 == server) {
		// child continues here

		// redirect stdin
		if (dup2(stdin[PIPE_READ], STDIN_FILENO) == -1) {
			exit(-1);
		}

		// redirect stdout
		if (dup2(stdout[PIPE_WRITE], STDOUT_FILENO) == -1) {
			exit(-1);
		}

		// redirect stderr
		if (dup2(stdout[PIPE_WRITE], STDERR_FILENO) == -1) {
			exit(-1);
		}

		// all these are for use by parent only
		close(stdin[PIPE_READ]);
		close(stdin[PIPE_WRITE]);
		close(stdout[PIPE_READ]);
		close(stdout[PIPE_WRITE]); 

		// run child process image
		// replace this with any exec* function find easier to use ("man exec")
		exitCode = execve(szCommand, aArguments, aEnvironment);

		// if we get here at all, an error occurred, but we are in the child
		// process, so just exit
		exit(exitCode);
	} else if (nChild > 0) {
		// parent continues here

		// close unused file descriptors, these are for child only
		close(stdin[PIPE_READ]);
		close(stdout[PIPE_WRITE]); 

		// done with these in this example program, you would normally keep these
		// open of course as long as you want to talk to the child
		
	} else {
		// failed to create child
		close(aStdinPipe[PIPE_READ]);
		close(aStdinPipe[PIPE_WRITE]);
		close(aStdoutPipe[PIPE_READ]);
		close(aStdoutPipe[PIPE_WRITE]);
	}
	std::cout << "Server started." << std::endl;
}
void Connection::stopServer()
{
	sendCommand("say SERVER SHUTTING DOWN IN 10 SECONDS.");
	for (int i=10; i>0; --i) {
		sendCommand("say "+std::to_string(i));
		std::this_thread::sleep_for (std::chrono::seconds(1));
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
	std::stringstream output;
	write(serverstdin[PIPE_WRITE], command.c_str(), stren(command.c_str));
	while (read(serverstdout[PIPE_READ], output.str().c_str(), 1) == 1) {
		write(STDOUT_FILENO, &nChar, 1);
	}
	return output;
}
