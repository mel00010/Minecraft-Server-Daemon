# Minecraft Server Daemon

About
-----
This is a pure C++ program designed to manage one or more Minecraft servers using the command line.  

Dependencies
------------
This program depends on these packages:
* GCC
* libpstreams-dev
* liblog4cpp-dev
* liblog4cpp5v5

Installing
--------

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

    
