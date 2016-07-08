/*
 * AbstractServerBase.hpp
 * 
 * Copyright 2016 Mel McCalla <melmccalla@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
 
#ifndef SERVER_H
#define SERVER_H

#include <event2/event.h>
#include <event2/event-config.h>
#include <event2/util.h>
#include <pwd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iosfwd>
#include <iostream>
#include <iterator>
#include <mutex>
#include <ostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "log4cpp/Category.hh"
#include "ServerPropertyFileParser.hpp"

struct event;

namespace MinecraftServerService {
class Server
{
	public:
		enum PIPE_OPS 
		{
			PIPE_READ = 0,
			PIPE_WRITE = 1,
		};
		typedef enum
		{
			GENERIC,
			VANILLA,
			BUKKIT,
			SPIGOT,
			BUNGEECORD,
		} ServerType;
		struct Listener
		{
			std::string* callbackOutput;
			size_t lines;
			bool persistent;
			size_t currentLine;
			std::string output;
		};
		struct ServerOutputEventData
		{
			struct event_base* base;
			log4cpp::Category* log;
			std::vector<Listener*>* listeners;
		};
		typedef Server& (*ServerManipulator)(Server&);
		typedef std::basic_ostream<char, std::char_traits<char> > CoutType;
		typedef CoutType& (*StandardEndLine)(CoutType&);
		
	public:
		virtual void updateServer(std::string version) = 0;
		virtual void backupServer() = 0;
		virtual void backupServer(std::string backupPath) = 0;
		virtual void serverStatus() = 0;
		virtual void startServer() = 0;
		virtual void stopServer() = 0;
		virtual void restartServer() = 0;
		virtual void reloadServer() = 0;
		virtual std::string listOnlinePlayers() = 0;
		virtual bool listOnlinePlayers(std::string playerName) = 0;
		virtual void sendCommand(std::string command) = 0;
		virtual std::string getServerName() = 0;
		virtual ServerType getServerType() = 0;
		virtual ServerPropertiesParser getServerPropertiesParser() = 0;
	protected:
		int serverPID = -1;
		int childProcessUID;
		int childProcessGID;
		int childProcessStdin[2];
		int childProcessStdout[2];
		log4cpp::Category* log;
		struct event_base* base = event_base_new();
		std::vector<Listener*>* listeners = new std::vector<Listener*>;
	public:
		inline bool isRunning() {
			if(serverPID != -1)
			{
				while(waitpid(-1, 0, WNOHANG) > 0) {
					// Wait for defunct....
				}
				if (0 == kill(serverPID, 0)){
					return 1; // Process exists
				} else {
					return 0;
				}
			} else {
				return 0;
			}
		};
		template <typename T>
		inline Server& operator<<(const T& x)
		{
			write(childProcessStdin[PIPE_WRITE], x, sizeof(x));
			return *this;
		}
		inline Server& operator<<(const std::string& x)
		{
			write(childProcessStdin[PIPE_WRITE], x.c_str(), x.size());
			return *this;
		}
		inline std::istream& operator>> (std::istream &in)
		{
			std::string s(std::istreambuf_iterator<char>(in), {});
			write(childProcessStdin[PIPE_WRITE], s.c_str(), s.size());
			return in;
		}
		// define an operator<< to take in std::endl
		inline Server& operator<<(StandardEndLine manip)
		{
			// call the function, but we cannot return it's value
			write(childProcessStdin[PIPE_WRITE], "\n", sizeof("\n"));
			return *this;
		}

#if DEBUGGING == 0		
	protected:
		static void outputListenerThread(int serverPID, int childProcessStdout, struct event_base* base, log4cpp::Category* log, std::vector<Listener*>* listeners) {
			log->debug("Server::outputListenerThread");
			struct event *outputListener;
			ServerOutputEventData* data = new ServerOutputEventData{base, log, listeners};
			outputListener = event_new(base, (evutil_socket_t)childProcessStdout, EV_READ|EV_PERSIST, Server::serverOutputEvent, (void*)data);
			event_add(outputListener, NULL);
			event_base_dispatch(base);
		}
		static void serverOutputEvent(evutil_socket_t fd, short event, void *arg) {
			//~ ((ServerOutputEventData*)arg)->log->info("Server::fifo_read");
			char buf[262144];
			int len;
			len = read(fd, buf, sizeof(buf) - 1);
			//~ ((ServerOutputEventData*)arg)->log->info("Read childProcessStdout into buf");
			if (len <= 0) {
				if (len == -1) {
					((ServerOutputEventData*)arg)->log->fatal("Error reading");
				} else if (len == 0) {
					((ServerOutputEventData*)arg)->log->fatal("Connection closed");
					event_base_loopbreak(((ServerOutputEventData*)arg)->base);
					return;
				}
			}
			buf[len] = '\0';
			//~ ((ServerOutputEventData*)arg)->log->info("Wrote null to end of buf");
			char* c = buf;
		    char* chars_array = strtok(buf, "\n");
		    while(chars_array != NULL)
		    {
				if (strlen(chars_array) > 0) {
					if(strchr(chars_array, '%')==NULL){
						((ServerOutputEventData*)arg)->log->info(chars_array);
						for( std::vector<Listener*>::iterator i = ((ServerOutputEventData*)arg)->listeners->begin() ; i != ((ServerOutputEventData*)arg)->listeners->end(); i++)
						{
							if((*i)->currentLine == (*i)->lines)
							{
								*((*i)->callbackOutput) = (*i)->output;
								if (!(*i)->persistent){
									((ServerOutputEventData*)arg)->listeners->erase(i);
								} else {
									(*i)->currentLine = 0;
									(*i)->output = '\0';
									(*i)->callbackOutput ='\0';
								}
							} else {
								(*i)->output = (*i)->output + std::string(chars_array);
								(*i)->currentLine++;
							}
						}
					} else {
						std::string buffer(chars_array);
						size_t position;
						std::string escapeBuffer(buffer);
						position = buffer.rfind("%");
						while (position != std::string::npos){
							escapeBuffer.replace (position, 1,"\%");
							buffer.erase(position);
							position = buffer.rfind("%");
						}
						((ServerOutputEventData*)arg)->log->info(escapeBuffer);
						for( std::vector<Listener*>::iterator i = ((ServerOutputEventData*)arg)->listeners->begin() ; i != ((ServerOutputEventData*)arg)->listeners->end(); i++)
						{
							if((*i)->currentLine == (*i)->lines)
							{
								*((*i)->callbackOutput) = (*i)->output;
								if (!(*i)->persistent){
									((ServerOutputEventData*)arg)->listeners->erase(i);
								} else {
									(*i)->currentLine = 0;
									(*i)->output = '\0';
									(*i)->callbackOutput ='\0';
								}
							} else {
								(*i)->output = (*i)->output + std::string(chars_array);
								(*i)->currentLine++;
							}
						}
					}
				}
		        chars_array = strtok(NULL, "\n");
		    }
		    return;
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
			std::vector<std::string> javaArgs, std::vector<std::string> serverOptions
		) 
		{
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
			fcntl(childProcessStdout[PIPE_READ], F_SETFL, O_NONBLOCK);
			//~ int server;
			serverPID = fork();
			if (serverPID == 0) {
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
				//~ setgid(childProcessGID);
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
			} else if (serverPID > 0) {
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
#else
	protected:
		static void outputListenerThread(int serverPID, int childProcessStdout, struct event_base* base, 
			log4cpp::Category* log, std::vector<Listener*>* listeners
		);
		static void serverOutputEvent(evutil_socket_t fd, short event, void *arg);
		void getUIDAndGIDFromUsername(const char* user);
		void launchServerProcess(std::string serverPath, std::string serverJarName, std::string serverAccount,
			int maxHeapAlloc, int minHeapAlloc, int gcThreadCount,
			std::vector<std::string> javaArgs, std::vector<std::string> serverOptions
		);
#endif /* DEBUGGING == 0 */
};
}
#endif /* SERVER_H */
