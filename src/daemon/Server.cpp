/*******************************************************************************
 *
 * Minecraft Server Daemon
 * Server.cpp
 * Copyright (C) 2016  Mel McCalla <melmccalla@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 *
 *******************************************************************************/

#include <fcntl.h>
#include <pwd.h>
#include <Server.hpp>
#include <cerrno>
#include <cstdlib>
#include <cstring>

namespace MinecraftServerDaemon {
/**
 * Function launched as a thread to listen for output on the server process.
 * Uses libevent to call Server::serverOutputEvent when there is data on the server process's output pipe.
 * @param serverPID
 * @param childProcessStdout
 * @param base
 * @param log
 * @param listeners
 */
void Server::outputListenerThread(__attribute__((unused)) int serverPID, int childProcessStdout, struct event_base* base, log4cpp::Category* log,
		std::vector<Listener*>* listeners) {
	log->debug("Server::outputListenerThread");
	struct event *outputListener;
	ServerOutputEventData* data = new ServerOutputEventData { base, log, listeners };
	outputListener = event_new(base, (evutil_socket_t) childProcessStdout, EV_READ | EV_PERSIST, Server::serverOutputEvent, (void*) data);
	event_add(outputListener, NULL);
	event_base_dispatch(base);
}
/**
 * Gets data from child process's output pipe, splits it into lines, and distributes it to the functions waiting for server output.
 * @param fd
 * @param event
 * @param arg
 */
void Server::serverOutputEvent(evutil_socket_t fd, __attribute__((unused)) short event, void *arg) {
	//~ ((ServerOutputEventData*)arg)->log->info("Server::fifo_read");
	char buf[262144];
	int len;
	len = read(fd, buf, sizeof(buf) - 1);
	//~ ((ServerOutputEventData*)arg)->log->info("Read childProcessStdout into buf");
	if (len <= 0) {
		if (len == -1) {
			((ServerOutputEventData*) arg)->log->fatal("Error reading");
		} else if (len == 0) {
			((ServerOutputEventData*) arg)->log->fatal("Connection closed");
			event_base_loopbreak(((ServerOutputEventData*) arg)->base);
			return;
		}
	}
	buf[len] = '\0';
	//~ ((ServerOutputEventData*)arg)->log->info("Wrote null to end of buf");
	char* chars_array = strtok(buf, "\n");
	while (chars_array != NULL) {
		if (strlen(chars_array) > 0) {
			if (strchr(chars_array, '%') == NULL) {
				((ServerOutputEventData*) arg)->log->info(chars_array);
				for (std::vector<Listener*>::iterator i = ((ServerOutputEventData*) arg)->listeners->begin();
						i != ((ServerOutputEventData*) arg)->listeners->end(); i++) {
					if ((*i)->currentLine == (*i)->lines) {
						*((*i)->callbackOutput) = (*i)->output;
						if (!(*i)->persistent) {
							((ServerOutputEventData*) arg)->listeners->erase(i);
						} else {
							(*i)->currentLine = 0;
							(*i)->output = '\0';
							(*i)->callbackOutput = '\0';
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
				while (position != std::string::npos) {
					escapeBuffer.replace(position, 1, "\%");
					buffer.erase(position);
					position = buffer.rfind("%");
				}
				((ServerOutputEventData*) arg)->log->info(escapeBuffer);
				for (std::vector<Listener*>::iterator i = ((ServerOutputEventData*) arg)->listeners->begin();
						i != ((ServerOutputEventData*) arg)->listeners->end(); i++) {
					if ((*i)->currentLine == (*i)->lines) {
						*((*i)->callbackOutput) = (*i)->output;
						if (!(*i)->persistent) {
							((ServerOutputEventData*) arg)->listeners->erase(i);
						} else {
							(*i)->currentLine = 0;
							(*i)->output = '\0';
							(*i)->callbackOutput = '\0';
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
/**
 * Takes a username and gets its user id and group id from it.
 * It stores this information in the member variables childProcessUID and childProcessGID.
 * @param user
 */
void Server::getUIDAndGIDFromUsername(const char* user) {
	struct passwd *pwd = new passwd[sizeof(struct passwd)]();
	size_t buffer_len = sysconf(_SC_GETPW_R_SIZE_MAX) * sizeof(char);
	char buffer[buffer_len];
	getpwnam_r(user, pwd, buffer, buffer_len, &pwd);
	if (pwd == NULL) {
		log->fatal("getpwnam_r failed to find username!! Does the user exist!!");
		throw;
	}
	childProcessUID = pwd->pw_uid;
	childProcessGID = pwd->pw_gid;
}
/**
 * Launches the server process and redirects its stdin, stdout, and stderr into pipes.
 * It stores the file descriptors for these pipes in childProcessStdin[PIPE_WRITE] and  childProcessStdout[PIPE_READ].
 * (Stdout and stderr share a pipe.)
 * @param serverPath
 * @param serverJarName
 * @param serverAccount
 * @param maxHeapAlloc
 * @param minHeapAlloc
 * @param gcThreadCount
 * @param javaArgs
 * @param serverOptions
 */
void Server::launchServerProcess(std::string serverPath, std::string serverJarName, std::string serverAccount, int maxHeapAlloc, int minHeapAlloc,
		int gcThreadCount, std::vector<std::string> javaArgs, std::vector<std::string> serverOptions) {
	int result;
	log->debug("ServerProcessBuf::createChild");
	getUIDAndGIDFromUsername((char*) serverAccount.c_str());
	int javaArgsSize = javaArgs.size();
	int serverOptionsSize = serverOptions.size();
	int arraySize = (javaArgsSize + serverOptionsSize + 7);
	char* serverCommand[arraySize];
	std::string _maxHeapAlloc = "-Xmx" + std::to_string(maxHeapAlloc) + "M";
	std::string _minHeapAlloc = "-Xms" + std::to_string(minHeapAlloc) + "M";
	std::string _gcThreadCount = "-XX:ParallelGCThreads=" + std::to_string(gcThreadCount);
	serverCommand[0] = (char*) "/usr/bin/java";
	serverCommand[1] = (char*) _maxHeapAlloc.c_str();
	serverCommand[2] = (char*) _minHeapAlloc.c_str();
	serverCommand[3] = (char*) _gcThreadCount.c_str();
	size_t i = 0;
	for (; i < javaArgs.size(); ++i) {
		javaArgs[i] = "-" + javaArgs[i];
		serverCommand[i + 4] = (char*) javaArgs[i].c_str();
	}
	serverCommand[i + 4] = (char*) "-jar";
	std::string _serverJarPath;
	_serverJarPath = serverPath + "/" + serverJarName;
	serverCommand[i + 5] = (char*) _serverJarPath.c_str();
	size_t j = 0;
	for (; j < serverOptions.size(); ++j) {
		serverCommand[j + i + 6] = (char*) serverOptions[j].c_str();
	}
	serverCommand[i + j + 7] = NULL;
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
			log->fatal("An error occurred while redirecting stdin.");
			exit(-1);
		}

		// redirect stdout
		if (dup2(childProcessStdout[PIPE_WRITE], STDOUT_FILENO) == -1) {
			log->fatal("An error occurred while redirecting stdout.");
			exit(-1);
		}

		// redirect stderr
		if (dup2(childProcessStdout[PIPE_WRITE], STDERR_FILENO) == -1) {
			log->fatal("An error occurred while redirecting stderr.");
			exit(-1);
		}

		// all these are for use by parent only
		close(childProcessStdin[PIPE_READ]);
		close(childProcessStdin[PIPE_WRITE]);
		close(childProcessStdout[PIPE_READ]);
		close(childProcessStdout[PIPE_WRITE]);
//		setgid(childProcessGID);
		setuid(childProcessUID);
		// run child process image
		result = execv("/usr/bin/java", serverCommand);

		// If we get here at all, an error occurred, but we are in the child
		// process, so just exit.
		log->fatal("Server process crashed with error ");
		log->fatal(std::to_string(result));
		log->fatal(std::to_string(errno));
		throw result;
	} else if (serverPID > 0) {
		// Parent continues here

		// Close unused file descriptors, these are for child only.
		close(childProcessStdin[PIPE_READ]);
		close(childProcessStdout[PIPE_WRITE]);
	} else {
		// Failed to create child
		close(childProcessStdin[PIPE_READ]);
		close(childProcessStdin[PIPE_WRITE]);
		close(childProcessStdout[PIPE_READ]);
		close(childProcessStdout[PIPE_WRITE]);
	}
}

} /* namespace MinecraftServerDaemon */
