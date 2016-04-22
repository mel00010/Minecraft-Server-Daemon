#include <iostream>
#include "server.h"

constexpr unsigned int str2int(const char* str, int h = 0)
{
    return !str[h] ? 5381 : (str2int(str, h+1) * 33) ^ str[h];
}

int main(int argc, char **argv)
{
	if(argc != 1)
	{
		switch (str2int(argv[1]))
		{
			case str2int("start"):
			{
				Server server1(std::string("minecraft"),std::string("/home/minecraft"));
				break;
			}
			case str2int("stop"):
			{
				Server server2(std::string("minecraft"));
				server2.stopServer();
				break;
			}
			case str2int("restart"):
			{
				Server server3(std::string("minecraft"));
				server3.stopServer();
				server3.startServer();
				break;
			}
			case str2int("update"):
			{
				std::cout << "Update functionality coming soon." << std::endl;
				break;
			}
			case str2int("backup"):
			{
				Server server4(std::string("minecraft"));
				server4.backupServer();
				break;
			}
			case str2int("status"):
			{
				std::cout << "Status functionality coming soon." << std::endl;
				break;
			}
			case str2int("command"):
			{
				std::cout << "Command functionality coming soon." << std::endl;
				break;
			}
			case str2int("listplayers"):
			{
				Server server6(std::string("minecraft"));
				server6.connection.listOnlinePlayers(std::string("Mel00010"));
				break;
			}
		}
		return 0;
	}
	else
	{
		std::cout << "Usage: "<< argv[0] << "{start|stop|backup|status|restart|listplayers|command \"server command\"}" << std::endl;
		return 0;	
	}
}

