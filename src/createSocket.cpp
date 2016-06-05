#include "createSocket.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include "log4cpp/Category.hh"
#include <event2/event.h>
int createSocket(log4cpp::Category& root)
{
	int controlSocket;
	int len;
	struct sockaddr_un local;
	if ((controlSocket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		root.fatal("Failure creating socket");
		exit(1);
	}
	
	local.sun_family = AF_UNIX;
	strcpy(local.sun_path, "/etc/minecraft/control.socket");
	unlink(local.sun_path);
	len = strlen(local.sun_path) + sizeof(local.sun_family);
	if (bind(controlSocket, (struct sockaddr *)&local, len) == -1) {
		root.fatal("Failure binding socket to path");
		exit(1);
	}
	
	if (listen(controlSocket, 5) == -1) {
		root.fatal("Failure listening on socket");
		exit(1);
	}
	fcntl(controlSocket, F_SETFL, O_NONBLOCK);
	return controlSocket;
}
