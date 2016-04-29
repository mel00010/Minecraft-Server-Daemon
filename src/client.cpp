#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

int fd, fd2, rc;
char buff[4];
char *pipePath = "/etc/minecraft/control.pipe";	
char *outputPipePath = "/etc/minecraft/output.pipe";

void help(char* argv0)
{
	std::cout << "Usage:  " << argv0 << " list|start <SERVER>|stop <SERVER>|restart <SERVER>|sendcommand <SERVER> <COMMAND>|status <SERVER>|update <SERVER>|backup <SERVER>|listplayers <SERVER> [PLAYER]|stopall|restartall|updateall|backupall|stopdaemon" << std::endl;
}
void writeToPipe(std::string command)
{
	std::ostringstream ss;
	ss << std::setw(4) << std::setfill('0') << command.size();
	std::string buf = ss.str() + command;
	write(fd, buf.c_str(), buf.size());
}
std::string readFromPipe()
{
	rc=read(fd2,buff,4);
	int size = atoi(buff);
	char line[size];
	rc = read(fd2,line,size);
	line[rc] = '\0';
	return std::string(line);
}
int main(int argc, char *argv[])
{
	if (argc > 1)
	{
		//~ mkfifo(outputPipePath, 0666);
		fd = open(pipePath, O_WRONLY);
		fd2 = open(outputPipePath, O_RDONLY | O_NONBLOCK);
	    std::string option(argv[1]);
		if (option == "list"){
			writeToPipe("listServers");
			sleep(1);
			std::string response(readFromPipe());
			std::cout << response << std::endl;
		} else if (option == "start") {
			if (argc > 2){
				writeToPipe("startServer");
				writeToPipe(argv[2]);
				sleep(1);
				
				std::cout << readFromPipe() << std::endl;
			}
		} else if (option == "stop") {
			if (argc > 2){
				writeToPipe("stopServer");
				writeToPipe(argv[2]);
				sleep(1);
				std::cout << readFromPipe() << std::endl;
			}
		} else if (option == "restart") {
			if (argc > 2){
				writeToPipe("restartServer");
				writeToPipe(argv[2]);
				sleep(1);
				std::cout << readFromPipe() << std::endl;
			}
		} else if (option == "sendcommand") {
			if (argc > 3){
				writeToPipe("sendCommand");
				writeToPipe(argv[2]);
				writeToPipe(argv[3]);
				sleep(1);
				std::cout << readFromPipe() << std::endl;
			}
		} else if (option == "status") {
			if (argc > 2){
				writeToPipe("status");
				writeToPipe(argv[2]);
				sleep(1);
				std::cout << readFromPipe() << std::endl;
			}
		} else if (option == "update") {
			if (argc > 2){
				writeToPipe("updateServer");
				writeToPipe(argv[2]);
				sleep(1);
				std::cout << readFromPipe() << std::endl;
			}
		} else if (option == "backup") {
			if (argc > 2){
				writeToPipe("backupServer");
				writeToPipe(argv[2]);
				sleep(1);
				std::cout << readFromPipe() << std::endl;
			}
		} else if (option == "listplayers") {
			if (argc > 3){
				writeToPipe("listOnlinePlayersFiltered");
				writeToPipe(argv[2]);
				writeToPipe(argv[3]);
			} else if (argc > 2){
				writeToPipe("listOnlinePlayers");
				writeToPipe(argv[2]);
				sleep(1);
				std::cout << readFromPipe() << std::endl;
			}
		} else if (option == "stopall") {
			writeToPipe("stopAll");
			sleep(1);
			std::cout << readFromPipe() << std::endl;
		} else if (option == "restartall") {
			writeToPipe("restartAll");
			sleep(1);
			std::cout << readFromPipe() << std::endl;
		} else if (option == "updateall") {
			writeToPipe("updateAll");
			sleep(1);
			std::cout << readFromPipe() << std::endl;
		} else if (option == "backupall") {
			writeToPipe("backupAll");
			sleep(1);
			std::cout << readFromPipe() << std::endl;
		} else if (option == "stopdaemon") {
			writeToPipe("stopDaemon");
			sleep(1);
			std::cout << readFromPipe() << std::endl;
		} else {
			std::cout << "Error:  argument " << option << " not found" << std::endl;
			help(argv[0]);
			return 1;
		}
		close(fd);
		return 0;
	} else {
		help(argv[0]);
		return 1;
	}
}
