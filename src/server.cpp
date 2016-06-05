#include "server.h"
#include "log4cpp/Category.hh"
#include "connection.h"
#include <pstreams/pstream.h>
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <time.h>
namespace MinecraftServerService {

Server::Server( std::string _serverName, std::string _serverPath, std::string _serverJarName, std::string _serverAccount,
				int _maxHeapAlloc, int _minHeapAlloc, int _gcThreadCount,
				std::string _backupPath, std::vector<std::string> _worldsToBackup, std::vector<std::string> _javaArgs, 
				std::vector<std::string> _serverOptions)
{
	serverName = _serverName;
	serverPath = _serverPath;
	serverJarName = _serverJarName;
	serverAccount = _serverAccount;
	maxHeapAlloc = _maxHeapAlloc;
	minHeapAlloc = _minHeapAlloc;
	gcThreadCount = _gcThreadCount;
	backupPath = _backupPath;
	worldsToBackup = _worldsToBackup;
	javaArgs = _javaArgs;
	serverOptions = _serverOptions;
	//~ log4cpp::PropertyConfigurator::configure("/etc/minecraft/log4cpp.properties");
	//~ serverProcess = new std::iostream();
	log = &log4cpp::Category::getInstance(std::string("server"));
	log->info(serverJarName);
	log->debug("Server::Server");
}
Server::~Server()
{
	log->debug("Server::~Server");
}
void Server::updateServer()
{
	log->debug("Server::updateServer");
}
void Server::updateServer(std::string serverPath, std::string serverJarName, std::string serverAccount)
{
	log->debug("Server::updateServer");
}
void Server::backupServer()
{
	log->debug("Server::backupServer");
	log->info("Starting backup");
	*serverProcess << "say SERVER BACKUP STARTING. Server going readonly..." << std::endl;
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
	*serverProcess << "say SERVER BACKUP ENDED. Server going read-write..." << std::endl;
	log->info("Backup finished");
}
void Server::backupServer(std::string _backupPath)
{
	log->debug("Server::backupServer");
	log->info("Starting backup");
	*serverProcess << "say SERVER BACKUP STARTING. Server going readonly..." << std::endl;
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
	*serverProcess << "say SERVER BACKUP ENDED. Server going read-write..." << std::endl;
	log->info("Backup finished");
}
void Server::backupServer(std::string _serverPath, std::string _serverAccount, std::string _backupPath, std::vector<std::string> _worldsToBackup)
{
	log->debug("Server::backupServer");
}
void Server::startServer()
{
	log->debug("Server::startServer");
	if (serverProcess == nullptr)
	{
		try {
			ServerProcessBuf* serverStreamBuf = new ServerProcessBuf(serverName, serverPath, serverJarName, serverAccount, maxHeapAlloc, minHeapAlloc, gcThreadCount, javaArgs, serverOptions);
			serverProcess = new std::iostream(serverStreamBuf);
			std::thread logger(&Server::logger, this);
			logger.detach();
		} 
		catch (...) {
			log->fatal("Exception occurred while starting server.  Exiting daemon. ");
			exit(1);
		}
	}
}
void Server::stopServer()
{
	log->debug("Server::stopServer");
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
		log->info("Server stopped");
	} else {
		log->info("Server already stopped");
	}
}
void Server::serverStatus()
{
	log->debug("Server::serverStatus");
}
void Server::restartServer()
{
	log->debug("Server::restartServer");
	//~ if (serverProcess != nullptr && !serverProcess->rdbuf()->exited())
	if (serverProcess != nullptr)
	{
		stopServer();
		startServer();
	}
}
void Server::sendCommand(std::string command)
{
	log->debug("Server::sendCommand");
	//~ if (serverProcess != nullptr && !serverProcess->rdbuf()->exited())
	if (serverProcess != nullptr)
	{
		*serverProcess << command << std::endl;
	}
}

void Server::listOnlinePlayers()
{
	log->debug("Server::listOnlinePlayers");
	
	
	//~ std::stringstream output;
	//~ std::string line;
	//~ int numPlayers = 0;
	
	//~ std::stringstream playerList = connection->*serverProcess << "list");
	//~ std::stringbuf *listBuf = playerList.rdbuf();
	
	//~ std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	//~ while (listBuf->in_avail() != 0)
	//~ {
		//~ getline(playerList, line);
		//~ output << line << std::endl;
		//~ numPlayers++;
	//~ }
	//~ if (numPlayers != 0) {
		//~ std::string outputBuf = output.str();
		//~ log->info(outputBuf);
	//~ } else {
		//~ log->info("No one is on the server");
	//~ }
}
void Server::listOnlinePlayers(std::string playerName)
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
//~ std::string Server::getOutput(int timeout, int lines)
//~ {
	//~ std::stringstream _output;
    //~ std::future_status status;
    //~ for(int i = 0; i < lines - 1; i++){
		//~ std::future<std::string> future = std::async(std::launch::async, [this](){ 
			//~ std::string __output;
			//serverOutput->mutex.lock();
			//std::getline(serverOutput->output, output);
			//~ std::getline(*output, __output);
			//serverOutput->mutex.unlock();
			//~ return __output;
		//~ }); 
        //~ status = future.wait_for(std::chrono::seconds(timeout));
        //~ if (status == std::future_status::timeout) {
            //~ break;
        //~ } else if (status == std::future_status::ready) {
            //~ _output << future.get();
        //~ }
	//~ }
    //~ return _output.str();
//~ }
void Server::logger()
{
	log->debug("ServerStream::logger");
	std::string _output;
	while (serverProcess != nullptr) {
	//~ while (true) {
		std::getline(*serverProcess, _output);
		if (_output.size() > 0) {
			log->info(_output);
		}
	}
}
}
