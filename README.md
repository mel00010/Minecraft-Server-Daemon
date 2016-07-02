# Minecraft Server Daemon

About
-----
This is a pure C++ program designed to manage one or more Minecraft servers using the command line.  It supports logging server output, backup, starting stopping and restarting server processes, and can list players on the server.  The servers it manages are configured through a json file.  

Dependencies
------------
This program depends on these packages:
* GCC
* liblog4cpp-dev
* liblog4cpp5v5
* libevent-core-2.0-5
* libevent-2.0-5
* libevent-dev

Building and installing
-----------------------

Installing the daemon is relatively standard procedure.  Just run these commands in a terminal:

	$ ./configure
	$ make
	$ sudo make install

Usage
-----

Using the program is pretty simple. To start the daemon, run this command in a terminal:  
	
	$ minecraftd
To control it, use the client program like this:

	$ minecraft <COMMAND>
The possible options for <COMMAND> are as follows:
* list:  Lists servers
* status \<SERVER\>:  Gets the current status of a server \<SERVER\>
* start \<SERVER\>:  Starts a server \<SERVER\>
* stop \<SERVER\>:  Stops a server \<SERVER\>
* stopall:  Stops all servers
* restart \<SERVER\>:  Retarts a server \<SERVER\>
* restartall:  Restarts all servers
* sendcommand \<SERVER\> <COMMAND>:  Sends a command <COMMAND> to a server \<SERVER\>
* update \<SERVER\>:  Updates a server \<SERVER\>
* updateall:  Updates all servers
* backup \<SERVER\>:  Backs up a server \<SERVER\>
* backupall:  Backs up all servers
* listplayers \<SERVER\> [PLAYER]:  Lists players on a server \<SERVER\>.  
* stopdaemon:  Stops the server daemon
Licensing
---------
This code is protected under the GPL version 2.

Contributing
------------
If you wish to contribute to this project, you can fork the project and send me a pull request for your changes.  
This project uses automake and autoconf for building.  To build a debug version (where the daemon is attached to the console), simply add --enable-debug to the end of your configure command.  
This project is not very well commented, however, commenting my code is on my to-do list.  
