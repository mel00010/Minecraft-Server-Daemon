#include "vanillaServer.h"
#include "log4cpp/Category.hh"
#include "ServerStream.h"
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <time.h>
#include <unistd.h>
#include <functional>
namespace MinecraftServerService {
VanillaServer::VanillaServer( std::string serverName, std::string serverPath, std::string serverJarName, std::string serverAccount,
				int maxHeapAlloc, int minHeapAlloc, int gcThreadCount,
				std::string backupPath, std::vector<std::string> worldsToBackup, std::vector<std::string> javaArgs, 
				std::vector<std::string> serverOptions) 
:	serverName{serverName}, serverPath{serverPath}, serverJarName{serverJarName}, serverAccount{serverAccount},
				maxHeapAlloc{maxHeapAlloc}, minHeapAlloc{minHeapAlloc}, gcThreadCount{gcThreadCount}, backupPath{backupPath},
				worldsToBackup{worldsToBackup}, javaArgs{javaArgs}, serverOptions{serverOptions}
{
	log = &log4cpp::Category::getInstance(std::string("server"));
	log->info(serverJarName);
	log->debug("VanillaServer::VanillaServer");
}
VanillaServer::~VanillaServer()
{
	log->debug("VanillaServer::~VanillaServer");
}
void VanillaServer::updateServer()
{
	log->debug("VanillaServer::updateServer");
}
void VanillaServer::backupServer()
{
	log->debug("VanillaServer::backupServer");
	log->info("Starting backup");
	*serverProcess << "say SERVER BACKUP STARTING. VanillaServer going readonly..." << std::endl;
	*serverProcess << "save-off" << std::endl;
	*serverProcess << "save-all" << std::endl;
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
	*serverProcess << "save-on" << std::endl;
	*serverProcess << "say SERVER BACKUP ENDED. VanillaServer going read-write..." << std::endl;
	log->info("Backup finished");
}
void VanillaServer::backupServer(std::string _backupPath)
{
	log->debug("VanillaServer::backupServer");
	log->info("Starting backup");
	*serverProcess << "say SERVER BACKUP STARTING. VanillaServer going readonly..." << std::endl;
	*serverProcess << "save-off" << std::endl;
	*serverProcess << "save-all" << std::endl;
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
	*serverProcess << "save-on" << std::endl;
	*serverProcess << "say SERVER BACKUP ENDED. VanillaServer going read-write..." << std::endl;
	log->info("Backup finished");
}
void VanillaServer::startServer()
{
	log->debug("VanillaServer::startServer");
	if (serverProcess == nullptr)
	{
		try {
			ServerProcessBuf* serverStreamBuf = new ServerProcessBuf(serverName, serverPath, serverJarName, serverAccount, maxHeapAlloc, minHeapAlloc, gcThreadCount, javaArgs, serverOptions);
			serverProcess = new std::iostream(serverStreamBuf);
			std::thread outputListenerThread(&Server::outputListenerThread, serverProcess, outputListeners, log, 1);
			outputListenerThread.detach();
			std::function<void(size_t, std::stringstream*, log4cpp::Category*)> f = std::bind( &VanillaServer::logger, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
			OutputListener outputListener(1, std::string("logger"), true, f);
			addOutputListener(outputListener);
			
		} 
		catch (...) {
			log->fatal("Exception occurred while starting server.  Exiting daemon. ");
			exit(1);
		}
	}
}
void VanillaServer::stopServer()
{
	log->debug("VanillaServer::stopServer");
	if (serverProcess != nullptr)
	{
		*serverProcess << "say SERVER SHUTTING DOWN IN 10 SECONDS." << std::endl;
		for (int i=10; i>0; --i) {
			*serverProcess << "say "+std::to_string(i) << std::endl;
			sleep(1);
		}
		*serverProcess << "say Saving map..." << std::endl;
		*serverProcess << "save-all" << std::endl;
		*serverProcess << "stop" << std::endl;
		//~ sleep(10);
		delete(serverProcess);
		//~ delete(serverProcess);
		serverProcess = NULL;
		log->info("VanillaServer stopped");
	} else {
		log->info("VanillaServer already stopped");
	}
}
void VanillaServer::serverStatus()
{
	log->debug("VanillaServer::serverStatus");
}
void VanillaServer::restartServer()
{
	log->debug("VanillaServer::restartServer");
	//~ if (serverProcess != nullptr && !serverProcess->rdbuf()->exited())
	if (serverProcess != nullptr)
	{
		stopServer();
		startServer();
	}
}
void VanillaServer::sendCommand(std::string command)
{
	log->debug("VanillaServer::sendCommand");
	//~ if (serverProcess != nullptr && !serverProcess->rdbuf()->exited())
	if (serverProcess != nullptr)
	{
		*serverProcess << command << std::endl;
	}
}

std::string VanillaServer::listOnlinePlayers()
{
	log->debug("VanillaServer::listOnlinePlayers");
	std::function<void(size_t, std::stringstream*, log4cpp::Category*)> f = std::bind( &VanillaServer::listOnlinePlayersCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	OutputListener outputListener(10, std::string("listOnlinePlayers"), false, f);
	addOutputListener(outputListener);
	*serverProcess << "list" << std::endl;
	while(callbackOutput == nullptr) { sleep(1); }
	if (callbackOutput->rdbuf()->in_avail() != 0) {
		log->info(callbackOutput->str());
		std::string returnValue(callbackOutput->str());
		delete callbackOutput;
		return returnValue;
	} else {
		log->info("No one is on the server");
		std::string returnValue = "No one is on the server";
		delete callbackOutput;
		return returnValue;
	}
}
void VanillaServer::listOnlinePlayersCallback(size_t linesRequested, std::stringstream* output, log4cpp::Category* log)
{
	std::stringbuf *listBuf = output->rdbuf();
	std::stringstream playersOnline;
	std::string line;
	int numPlayers = 0;
	
	//~ std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	while (listBuf->in_avail() != 0)
	{
		std::getline(*output, line);
		playersOnline << line << std::endl;
		numPlayers++;
	}
	if (numPlayers != 0) {
		std::stringstream* _callbackOutput = new std::stringstream();
		*_callbackOutput << playersOnline.str();
		callbackOutput=_callbackOutput;
	} else {
		std::stringstream* _callbackOutput = new std::stringstream();
		*_callbackOutput << playersOnline.str();
		callbackOutput=_callbackOutput;
	}
}
void VanillaServer::listOnlinePlayers(std::string playerName)
{
	//~ std::stringstream output;
	//~ std::string line;
	//~ int numPlayers = 0;
	
	//~ std::stringstream playerList = *serverProcess << "list");
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
void VanillaServer::logger(size_t linesRequested, std::stringstream* output, log4cpp::Category* log)
{
	//~ std::string _output;
	for (size_t i = 0; i < linesRequested; i++) {
		std::getline(*serverProcess, _output);
		if (_output.size() > 0) {
			log->info(_output);
		}
	}
}
}
