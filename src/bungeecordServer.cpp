#include "bungeecordServer.h"
#include "log4cpp/Category.hh"
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <time.h>
#include <unistd.h>
#include <functional>
namespace MinecraftServerService {
BungeeCordServer::BungeeCordServer( std::string serverName, std::string serverPath, std::string serverJarName, std::string serverAccount,
				int maxHeapAlloc, int minHeapAlloc, int gcThreadCount,
				std::string backupPath, std::vector<std::string> javaArgs, std::vector<std::string> serverOptions)
:	serverName{serverName}, serverPath{serverPath}, serverJarName{serverJarName}, serverAccount{serverAccount}, maxHeapAlloc{maxHeapAlloc},
				minHeapAlloc{minHeapAlloc}, gcThreadCount{gcThreadCount}, backupPath{backupPath}, javaArgs{javaArgs}, serverOptions{serverOptions}
{
	log = &log4cpp::Category::getInstance(serverName);
	log->info(serverJarName);
	log->debug("BungeeCordServer::BungeeCordServer");
}
BungeeCordServer::~BungeeCordServer()
{
	log->debug("BungeeCordServer::~BungeeCordServer");
}
void BungeeCordServer::updateServer()
{
	log->debug("BungeeCordServer::updateServer");
}
void BungeeCordServer::backupServer()
{
	log->debug("BungeeCordServer::backupServer");
	log->info("Starting backup");
	*this << "say SERVER BACKUP STARTING. BungeeCordServer going readonly..." << std::endl;
	*this << "save-off" << std::endl;
	*this << "save-all" << std::endl;
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	std::strftime(buf, sizeof(buf), "%Y-%m-%d_%Hh%M", &tstruct);
	std::string time(buf);
	for (std::string world : worldsToBackup)
	{
		log->info("Backing up "+ world);
		std::string tarCommand = "tar -C \""+serverPath+"\" -cf "+serverPath+"/"+backupPath+"/"+world+"_"+time+".tar\" "+world;
		std::string gzipCommand = "gzip -f \""+serverPath+"/"+backupPath+"/"+world+"_"+time+".tar\"";
		log->info(tarCommand);
		system(tarCommand.c_str());
		log->info(gzipCommand);
		system(gzipCommand.c_str());
	}
	log->info("Backing up "+serverJarName);
	std::string copyJarCommand = "cp \""+serverPath+"/"+serverJarName+"\" \""+backupPath+"/"+serverJarName.substr(0, serverJarName.size()-4)+"_"+time+".jar\"";
	log->info(copyJarCommand);
	system(copyJarCommand.c_str());
	*this << "save-on" << std::endl;
	*this << "say SERVER BACKUP ENDED. BungeeCordServer going read-write..." << std::endl;
	log->info("Backup finished");
}
void BungeeCordServer::backupServer(std::string _backupPath)
{
	log->debug("BungeeCordServer::backupServer");
	log->info("Starting backup");
	*this << "say SERVER BACKUP STARTING. BungeeCordServer going readonly..." << std::endl;
	*this << "save-off" << std::endl;
	*this << "save-all" << std::endl;
	time_t now = time(0);

	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	std::strftime(buf, sizeof(buf), "%Y-%m-%d_%Hh%M", &tstruct);
	std::string time(buf);
	for (std::string world : worldsToBackup)
	{
		log->info("Backing up "+world);
		std::string tarCommand = "tar -C \""+serverPath+"\" -cf "+_backupPath+"/"+world+"_"+time+".tar\" "+world;
		std::string gzipCommand = "gzip -f \""+backupPath+"/"+world+"_"+time+".tar\"";
		log->info(tarCommand);
		system(tarCommand.c_str());
		log->info(gzipCommand);
		system(gzipCommand.c_str());
	}
	log->info("Backing up "+serverJarName);
	std::string copyJarCommand = "cp \""+serverPath+"/"+serverJarName+"\" \""+_backupPath+"/"+serverJarName.substr(0, serverJarName.size()-4)+"_"+time+".jar\"";
	log->info(copyJarCommand);
	system(copyJarCommand.c_str());
	*this << "save-on" << std::endl;
	*this << "say SERVER BACKUP ENDED. BungeeCordServer going read-write..." << std::endl;
	log->info("Backup finished");
}
void BungeeCordServer::reloadServer()
{
	*this << "reload" << std::endl;
}
void BungeeCordServer::startServer()
{
	log->debug("BungeeCordServer::startServer");
	if (!isRunning())
	{
		chdir(serverPath.c_str());
		launchServerProcess(serverPath, serverJarName, serverAccount, maxHeapAlloc, minHeapAlloc, gcThreadCount, javaArgs, serverOptions);
		log->debug("Launched server process");			
		std::thread outputListenerThread(&Server::outputListenerThread, serverPID, childProcessStdout[PIPE_READ], base, log);
		outputListenerThread.detach();
	}
}
void BungeeCordServer::stopServer()
{
	log->debug("BungeeCordServer::stopServer");
	if (isRunning())
	{
		*this << "end" << std::endl;
		while(isRunning()) {
			sleep(0.5);
		}
		log->info("Server stopped");
	} else {
		log->info("Server already stopped");
	}
}
void BungeeCordServer::serverStatus()
{
	log->debug("BungeeCordServer::serverStatus");
}
void BungeeCordServer::restartServer()
{
	log->debug("BungeeCordServer::restartServer");
	//~ if (isRunning() && !serverProcess->rdbuf()->exited())
	if (isRunning())
	{
		stopServer();
		startServer();
	}
}
void BungeeCordServer::sendCommand(std::string command)
{
	log->debug("BungeeCordServer::sendCommand");
	//~ if (isRunning() && !serverProcess->rdbuf()->exited())
	if (isRunning())
	{
		*this << command << std::endl;
	}
}

std::string BungeeCordServer::listOnlinePlayers()
{
	//~ log->debug("BungeeCordServer::listOnlinePlayers");
	//~ std::function<void(size_t, std::stringstream*, log4cpp::Category*)> f = std::bind( &BungeeCordServer::listOnlinePlayersCallback, *this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	//~ OutputListener outputListener(10, std::string("listOnlinePlayers"), false, f);
	//~ addOutputListener(outputListener);
	//~ *this << "list" << std::endl;
	//~ while(callbackOutput == nullptr) { sleep(1); }
	//~ if (callbackOutput->rdbuf()->in_avail() != 0) {
		//~ log->info(callbackOutput->str());
		//~ std::string returnValue(callbackOutput->str());
		//~ delete callbackOutput;
		//~ return returnValue;
	//~ } else {
		//~ log->info("No one is on the server");
		//~ std::string returnValue = "No one is on the server";
		//~ delete callbackOutput;
		//~ return returnValue;
	//~ }
}
void BungeeCordServer::listOnlinePlayersCallback(size_t linesRequested, std::stringstream* output, log4cpp::Category* log)
{
	//~ std::stringbuf *listBuf = output->rdbuf();
	//~ std::stringstream playersOnline;
	//~ std::string line;
	//~ int numPlayers = 0;
	
	//~ // std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	//~ while (listBuf->in_avail() != 0)
	//~ {
		//~ std::getline(*output, line);
		//~ playersOnline << line << std::endl;
		//~ numPlayers++;
	//~ }
	//~ if (numPlayers != 0) {
		//~ std::stringstream* _callbackOutput = new std::stringstream();
		//~ *_callbackOutput << playersOnline.str();
		//~ callbackOutput=_callbackOutput;
	//~ } else {
		//~ std::stringstream* _callbackOutput = new std::stringstream();
		//~ *_callbackOutput << playersOnline.str();
		//~ callbackOutput=_callbackOutput;
	//~ }
}
void BungeeCordServer::listOnlinePlayers(std::string playerName)
{
	//~ std::stringstream output;
	//~ std::string line;
	//~ int numPlayers = 0;
	
	//~ std::stringstream playerList = *this << "list";
	//~ std::stringbuf *listBuf = playerList.rdbuf();
	
	//~ std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	//~ while (listBuf->in_avail() != 0)
	//~ {
		//~ getline(playerList, line);
		//~ if (line != playerName) {
			//~ output << line << std::endl;
			//~ numPlayers++;
		//~ }
	//~ }
	//~ if (numPlayers != 0) {
		//~ std::string outputBuf = output.str();
		//~ log->info(outputBuf);
	//~ } else {
		//~ log->info("No one is on the server");
	//~ }
}
}
