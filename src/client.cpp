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
//char *socket_path = "./socket";
int fd[2];
char *controlPipePath = "/etc/minecraft/control.pipe";
char *outputPipePath = "/etc/minecraft/output.pipe";

void help(char* argv0)
{
	std::cout << "Usage:  " << argv0 << " list|start <SERVER>|stop <SERVER>|restart <SERVER>|sendcommand <SERVER> <COMMAND>|status <SERVER>|update <SERVER>|backup <SERVER>|listplayers <SERVER> [PLAYER]|stopall|restartall|updateall|backupall" << std::endl;
}
void writeToPipe(std::string command)
{
	std::ostringstream ss;
	ss << std::setw(4) << std::setfill('0') << command.size();
	//~ std::string result = ss.str();
	std::string buf = ss.str() + command;
	write(fd[1], buf.c_str(), buf.size());
	//~ close(fd);
}
int main(int argc, char *argv[])
{
	if (argc > 1)
	{
		fd[0] = open(outputPipePath, O_RDONLY);
		fd[1] = open(controlPipePath, O_WRONLY);
	    std::string option(argv[1]);
		if (option == "list"){
			writeToPipe("listServers");
		} else if (option == "start") {
			if (argc > 2){
				writeToPipe("startServer");
				writeToPipe(argv[2]);
			}
		} else if (option == "stop") {
			if (argc > 2){
				writeToPipe("stopServer");
				writeToPipe(argv[2]);
			}
		} else if (option == "restart") {
			if (argc > 2){
				writeToPipe("restartServer");
				writeToPipe(argv[2]);
			}
		} else if (option == "sendcommand") {
			if (argc > 3){
				writeToPipe("sendCommand");
				writeToPipe(argv[2]);
				writeToPipe(argv[3]);
			}
		} else if (option == "status") {
			if (argc > 2){
				writeToPipe("status");
				writeToPipe(argv[2]);
			}
		} else if (option == "update") {
			if (argc > 2){
				writeToPipe("updateServer");
				writeToPipe(argv[2]);
			}
		} else if (option == "backup") {
			if (argc > 2){
				writeToPipe("backupServer");
				writeToPipe(argv[2]);
			}
		} else if (option == "listplayers") {
			if (argc > 3){
				writeToPipe("listOnlinePlayersFiltered");
				writeToPipe(argv[2]);
				writeToPipe(argv[3]);
			} else if (argc > 2){
				writeToPipe("listOnlinePlayers");
				writeToPipe(argv[2]);
			}
		} else if (option == "stopall") {
			writeToPipe("stopAll");
		} else if (option == "restartall") {
			writeToPipe("restartAll");
		} else if (option == "updateall") {
			writeToPipe("updateAll");
		} else if (option == "backupall") {
			writeToPipe("backupAll");
		} else {
			std::cout << "Error:  argument " << option << " not found" << std::endl;
			help(argv[0]);
			return 1;
		}
		close(fd[1]);
		return 0;
	} else {
		help(argv[0]);
		return 1;
	}
}
