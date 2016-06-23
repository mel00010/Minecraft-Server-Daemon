#include "bukkitServer.h"
#include "log4cpp/Category.hh"
#include "ServerStream.h"
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <time.h>
#include <unistd.h>
namespace MinecraftServerService {
BukkitServer::BukkitServer( std::string serverName, std::string serverPath, std::string serverJarName, std::string serverAccount,
				int maxHeapAlloc, int minHeapAlloc, int gcThreadCount,
				std::string backupPath, std::vector<std::string> worldsToBackup, std::vector<std::string> javaArgs, 
				std::vector<std::string> serverOptions) 
:	serverName{serverName}, serverPath{serverPath}, serverJarName{serverJarName}, serverAccount{serverAccount},
				maxHeapAlloc{maxHeapAlloc}, minHeapAlloc{minHeapAlloc}, gcThreadCount{gcThreadCount}, backupPath{backupPath},
				worldsToBackup{worldsToBackup}, javaArgs{javaArgs}, serverOptions{serverOptions}
{
	log = &log4cpp::Category::getInstance(std::string("server"));
	log->info(serverJarName);
	log->debug("BukkitServer::BukkitServer");
}
BukkitServer::~BukkitServer()
{
	log->debug("BukkitServer::~BukkitServer");
}
void BukkitServer::updateServer()
{
	log->debug("BukkitServer::updateServer");
}
void BukkitServer::backupServer()
{
	log->debug("BukkitServer::backupServer");
	log->info("Starting backup");
	*serverProcess << "say SERVER BACKUP STARTING. BukkitServer going readonly..." << std::endl;
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
	*serverProcess << "say SERVER BACKUP ENDED. BukkitServer going read-write..." << std::endl;
	log->info("Backup finished");
}
void BukkitServer::backupServer(std::string _backupPath)
{
	log->debug("BukkitServer::backupServer");
	log->info("Starting backup");
	*serverProcess << "say SERVER BACKUP STARTING. BukkitServer going readonly..." << std::endl;
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
	*serverProcess << "say SERVER BACKUP ENDED. BukkitServer going read-write..." << std::endl;
	log->info("Backup finished");
}
void BukkitServer::startServer()
{
	log->debug("BukkitServer::startServer");
	if (serverProcess == nullptr)
	{
		try {
			ServerProcessBuf* serverStreamBuf = new ServerProcessBuf(serverName, serverPath, serverJarName, serverAccount, maxHeapAlloc, minHeapAlloc, gcThreadCount, javaArgs, serverOptions);
			serverProcess = new std::iostream(serverStreamBuf);
			std::thread outputListenerThread(&Server::outputListenerThread, serverProcess, outputListeners, log, 5);
			outputListenerThread.detach();
			std::function<void(size_t, std::stringstream*, log4cpp::Category*)> f = std::bind( &BukkitServer::logger, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
			OutputListener outputListener(1, std::string("logger"), true, f);
			addOutputListener(outputListener);
		} 
		catch (...) {
			log->fatal("Exception occurred while starting server.  Exiting daemon. ");
			exit(1);
		}
	}
}
void BukkitServer::stopServer()
{
	log->debug("BukkitServer::stopServer");
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
		log->info("BukkitServer stopped");
	} else {
		log->info("BukkitServer already stopped");
	}
}
void BukkitServer::serverStatus()
{
	log->debug("BukkitServer::serverStatus");
}
void BukkitServer::restartServer()
{
	log->debug("BukkitServer::restartServer");
	//~ if (serverProcess != nullptr && !serverProcess->rdbuf()->exited())
	if (serverProcess != nullptr)
	{
		stopServer();
		startServer();
	}
}
void BukkitServer::reloadServer()
{
	*serverProcess << "reload" << std::endl;
}
void BukkitServer::sendCommand(std::string command)
{
	log->debug("BukkitServer::sendCommand");
	//~ if (serverProcess != nullptr && !serverProcess->rdbuf()->exited())
	if (serverProcess != nullptr)
	{
		*serverProcess << command << std::endl;
	}
}

std::string BukkitServer::listOnlinePlayers()
{
	log->debug("BukkitServer::listOnlinePlayers");
	
	
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
void BukkitServer::listOnlinePlayers(std::string playerName)
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
//~ std::string BukkitServer::getOutput(int timeout, int lines)
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
void BukkitServer::logger(size_t linesRequested, std::stringstream* output, log4cpp::Category* log)
{
	std::string _output;
	for (size_t i = 0; i < linesRequested; i++) {
		std::getline(*serverProcess, _output);
		if (_output.size() > 0) {
			log->info(_output);
		}
	}
}
}
