#ifndef SERVER_H
#define SERVER_H
#include "ServerStream.h"
#include "outputListener.h"
#include "log4cpp/Category.hh"
#include <string>
#include <sstream>
#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <time.h>
namespace MinecraftServerService {
class Server
{
	public:
		virtual void updateServer() = 0;
		virtual void backupServer() = 0;
		virtual void backupServer(std::string backupPath) = 0;
		virtual void serverStatus() = 0;
		virtual void startServer() = 0;
		virtual void stopServer() = 0;
		virtual void restartServer() = 0;
		virtual void reloadServer() = 0;
		virtual std::string listOnlinePlayers() = 0;
		virtual void listOnlinePlayers(std::string playerName) = 0;
		virtual void sendCommand(std::string command) = 0;
		std::string serverName;
	protected:
		void addOutputListener(OutputListener& outputListener)
		{
			if (outputListeners != nullptr) {
				std::shared_ptr<std::vector<OutputListener>> _outputListeners = outputListeners;
				{
					static std::mutex io_mutex;
					std::lock_guard<std::mutex> lk(io_mutex);
					_outputListeners.get()->push_back(outputListener);
				}
			}
		};
		void removeOutputListener(OutputListener& outputListener)
		{
			if (outputListeners != nullptr) {
				std::shared_ptr<std::vector<OutputListener>> _outputListeners = outputListeners;
				{
					static std::mutex io_mutex;
					std::lock_guard<std::mutex> lk(io_mutex);
					for (std::vector<OutputListener>::iterator it = outputListeners.get()->begin(); it != outputListeners.get()->end(); ++it) {
						if (outputListener.listenerName == it->listenerName) {
							outputListeners.get()->erase(it);
							break;
						}
					}
				}
			}
		};
		static void outputListenerThread(std::iostream* serverProcess, std::shared_ptr<std::vector<OutputListener>> _outputListeners, log4cpp::Category* log, size_t timeout = 5)
		{
			std::shared_ptr<std::vector<OutputListener>> outputListeners = _outputListeners;
			{
				static std::mutex io_mutex;
				log->debug("Server::outputListenerThread");
				std::string output;
				while (serverProcess != nullptr) {
					bool timedOut = true;
					time_t t = time(NULL) + timeout;
					while (time(NULL)<t) {
						if (serverProcess->peek()!=EOF) {
							timedOut = false;
							break;
						}
					}
					if (!timedOut)
					{
						std::getline(*serverProcess, output);
						//~ std::thread t([&]{
							if (output.size() > 0) {
								std::lock_guard<std::mutex> lk(io_mutex);
								for (std::vector<OutputListener>::iterator it = outputListeners.get()->begin(); it != outputListeners.get()->end(); ++it) {
									if (it->currentLine != it->linesRequested) {
										*(it->lines) << output;
										it->currentLine++;
									} else {
										it->callback(it->linesRequested, it->lines, log);
										if(!it->persistent){
											outputListeners.get()->erase(it);
										}
									}
								}
								log->info(output);
							}
						//~ });
						//~ t.join();
					} else {
						//~ std::lock_guard<std::mutex> lk(io_mutex);
						for (std::vector<OutputListener>::iterator it = outputListeners.get()->begin(); it != outputListeners.get()->end(); ++it) {
							it->callback(it->linesRequested, it->lines, log);
							if(!it->persistent){
								outputListeners.get()->erase(it);
							}
						}
					}
				}
			}
		};
		void startServer(
			std::string serverPath, std::string serverJarName, std::string serverAccount,
			int maxHeapAlloc, int minHeapAlloc, int gcThreadCount,
			std::vector<std::string> javaArgs, std::vector<std::string> serverOptions
		)
		{
			log->info("Starting " + serverJarName);
			chdir(serverPath.c_str());
			launchServerProcess(serverPath, serverJarName, serverAccount, maxHeapAlloc, minHeapAlloc, gcThreadCount, javaArgs, serverOptions);
			log->debug("Launched server process");
		}
		void getUIDAndGIDFromUsername(const char* user) {
		    struct passwd *pwd = new passwd[sizeof(struct passwd)]();
		    size_t buffer_len = sysconf(_SC_GETPW_R_SIZE_MAX) * sizeof(char);
		    char buffer[buffer_len];
			getpwnam_r(user, pwd, buffer, buffer_len, &pwd);
			if(pwd == NULL)
			{
				log->fatal("getpwnam_r failed to find username!! Does the user exist!!");
				throw;
			}
		    childProcessUID = pwd->pw_uid;
		    childProcessGID = pwd->pw_gid;
		}
		void launchServerProcess(std::string serverPath, std::string serverJarName, std::string serverAccount,
			int maxHeapAlloc, int minHeapAlloc, int gcThreadCount,
			std::vector<std::string> javaArgs, std::vector<std::string> serverOptions) {
			int result;
			log->debug("ServerProcessBuf::createChild");
			getUIDAndGIDFromUsername((char*)serverAccount.c_str());
			int javaArgsSize = javaArgs.size();
			int serverOptionsSize = serverOptions.size();
			int arraySize = (javaArgsSize+serverOptionsSize+7);
			char* serverCommand[arraySize];
			std::string _maxHeapAlloc = "-Xmx"+std::to_string(maxHeapAlloc)+"M";
			std::string _minHeapAlloc = "-Xms"+std::to_string(minHeapAlloc)+"M";
			std::string _gcThreadCount= "-XX:ParallelGCThreads="+std::to_string(gcThreadCount);
			serverCommand[0] = "/usr/bin/java";
			serverCommand[1] = (char*)_maxHeapAlloc.c_str();
			serverCommand[2] = (char*)_minHeapAlloc.c_str();
			serverCommand[3] = (char*)_gcThreadCount.c_str();
			size_t i = 0;
			for (;i < javaArgs.size(); ++i)
			{
				javaArgs[i]="-"+javaArgs[i];
				serverCommand[i+4]= (char*)javaArgs[i].c_str();
			}
			serverCommand[i+4] = "-jar";
			std::string _serverJarPath;
			_serverJarPath= serverPath+"/"+serverJarName;
			serverCommand[i+5] = (char*)_serverJarPath.c_str();
			size_t j = 0;
			for (; j < serverOptions.size(); ++j)
			{
				serverCommand[j+i+6]=(char*)serverOptions[j].c_str();
			}
			serverCommand[i+j+7]=NULL;
			if (pipe(childProcessStdin) < 0) {
				log->fatal("allocating pipe for child input redirect");
				exit(-1);
			}
			if (pipe(childProcessStdout) < 0) {
				close(childProcessStdin[PIPE_READ]);
				close(childProcessStdin[PIPE_WRITE]);
				log->fatal("allocating pipe for child output redirect");
				exit(-1);
			}
			//~ int server;
			server = fork();
			if (server == 0) {
				// child continues here
		
				// redirect stdin
				if (dup2(childProcessStdin[PIPE_READ], STDIN_FILENO) == -1) {
					log->fatal("redirecting stdin");
					exit(-1);
				}
		
				// redirect stdout
				if (dup2(childProcessStdout[PIPE_WRITE], STDOUT_FILENO) == -1) {
					log->fatal("redirecting stdout");
					exit(-1);
				}
		
				// redirect stderr
				if (dup2(childProcessStdout[PIPE_WRITE], STDERR_FILENO) == -1) {
					log->fatal("redirecting stderr");
					exit(-1);
				}
		
				//~ // all these are for use by parent only
				close(childProcessStdin[PIPE_READ]);
				close(childProcessStdin[PIPE_WRITE]);
				close(childProcessStdout[PIPE_READ]);
				close(childProcessStdout[PIPE_WRITE]); 
				setgid(childProcessGID);
				setuid(childProcessUID);
				// run child process image
				// replace this with any exec* function find easier to use ("man exec")
				execv("/usr/bin/java", serverCommand);
		
				// if we get here at all, an error occurred, but we are in the child
				// process, so just exit
				log->fatal("Server process crashed with error ");
				log->fatal(std::to_string(result));
				log->fatal(std::to_string(errno));
				throw result;
			} else if (server > 0) {
				// parent continues here
		
				// close unused file descriptors, these are for child only
				close(childProcessStdin[PIPE_READ]);
				close(childProcessStdout[PIPE_WRITE]); 
			} else {
				// failed to create child
				close(childProcessStdin[PIPE_READ]);
				close(childProcessStdin[PIPE_WRITE]);
				close(childProcessStdout[PIPE_READ]);
				close(childProcessStdout[PIPE_WRITE]);
			}
		}
		bool isRunning() {
			while(waitpid(-1, 0, WNOHANG) > 0) {
		        // Wait for defunct....
		    }
		
		    if (0 == kill(server, 0))
		        return 1; // Process exists
		
		    return 0;
		}
		void stopServer() {
			//~ if(!exited()) {
				log->notice("Server shutting down");
				while(!exited()) {}
				log->notice("Server stopped.");
			//~ }
		}
		std::shared_ptr<std::vector<OutputListener>> outputListeners = nullptr;
		int server;
        int childProcessUID;
        int childProcessGID;
        int childProcessStdin[2];
		int childProcessStdout[2];
};

}
#endif /* SERVER_H */
