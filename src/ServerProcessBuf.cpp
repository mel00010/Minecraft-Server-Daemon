// Copyright 2007 Edd Dawson.
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file BOOST_LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

#include "ServerProcessBuf.h"
#include "log4cpp/Category.hh"
#include "log4cpp/Appender.hh"
#include "log4cpp/FileAppender.hh"
#include "log4cpp/Layout.hh"
#include "log4cpp/PatternLayout.hh"
#include "log4cpp/Priority.hh"
#include <cctype>
#include <algorithm>
#include <iterator>
#include <streambuf>
#include <cstddef>
#include <unistd.h>
#include <ostream>
#include <functional>
#include <cassert>
#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <pwd.h>
#include <vector>
#include <algorithm>
#include <errno.h>
namespace MinecraftServerService {
ServerProcessBuf::ServerProcessBuf(std::string serverName, std::string serverPath, std::string serverJarName, std::string serverAccount,
			int maxHeapAlloc, int minHeapAlloc, int gcThreadCount,
			std::vector<std::string> javaArgs, std::vector<std::string> serverOptions)
{
	log4cpp::Appender *appender = new log4cpp::FileAppender(serverName, "/var/log/minecraft/servers/"+serverName+".log");
	log4cpp::PatternLayout* pattern = new log4cpp::PatternLayout();
	pattern->setConversionPattern("%d %c [%p] %m%n");
	appender->setLayout(pattern);
	log = &log4cpp::Category::getInstance(serverName);
	log->addAppender(appender);
	this->setg(this->inbuf_, this->inbuf_, this->inbuf_);
    this->setp(this->outbuf_, this->outbuf_ + bufsize - 1);
	startServer(serverPath, serverJarName, serverAccount, maxHeapAlloc, minHeapAlloc, gcThreadCount, javaArgs, serverOptions);
}

ServerProcessBuf::int_type ServerProcessBuf::overflow(int c)
{
    if (!traits_type::eq_int_type(c, traits_type::eof())) {
        *this->pptr() = traits_type::to_char_type(c);
        this->pbump(1);
    }
    return this->sync() == -1
        ? traits_type::eof()
        : traits_type::not_eof(c);
}
ServerProcessBuf::~ServerProcessBuf(){
	stopServer();
	close(childProcessStdin[PIPE_WRITE]);
	close(childProcessStdout[PIPE_READ]);
}
//~ int ServerProcessBuf::sync()
//~ {
	//~ std::ptrdiff_t n = pptr() - pbase();
    //~ pbump(-n);
    //~ return sink_.write(pbase(), n) ? 0 : -1;
//~ }
int ServerProcessBuf::underflow()
{
	if (this->gptr() == this->egptr()) {
        std::streamsize pback(std::min(this->gptr() - this->eback(),
                                       std::ptrdiff_t(16 - sizeof(int))));
        std::copy(this->egptr() - pback, this->egptr(), this->eback());
        int done(::read(this->childProcessStdout[PIPE_READ], this->eback() + pback, bufsize));
        this->setg(this->eback(),
                   this->eback() + pback,
                   this->eback() + pback + std::max(0, done));
    }
    return this->gptr() == this->egptr()
        ? traits_type::eof()
        : traits_type::to_int_type(*this->gptr());
}
int ServerProcessBuf::sync(){
	if (this->pbase() != this->pptr()) {
        std::streamsize size(this->pptr() - this->pbase());
        std::streamsize done(::write(this->childProcessStdin[PIPE_WRITE], this->outbuf_, size));
        // The code below assumes that it is success if the stream made
        // some progress. Depending on the needs it may be more
        // reasonable to consider it a success only if it managed to
        // write the entire buffer and, e.g., loop a couple of times
        // to try achieving this success.
        if (0 < done) {
            std::copy(this->pbase() + done, this->pptr(), this->pbase());
            this->setp(this->pbase(), this->epptr());
            this->pbump(size - done);
        }
    }
    return this->pptr() != this->epptr()? 0: -1;
}
void ServerProcessBuf::startServer(
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
void ServerProcessBuf::getUIDAndGIDFromUsername(const char* user) {
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
void ServerProcessBuf::launchServerProcess(std::string serverPath, std::string serverJarName, std::string serverAccount,
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
bool ServerProcessBuf::exited() {
	while(waitpid(-1, 0, WNOHANG) > 0) {
        // Wait for defunct....
    }

    if (0 == kill(server, 0))
        return 0; // Process exists

    return 1;
}
void ServerProcessBuf::stopServer() {
	//~ if(!exited()) {
		log->notice("Server shutting down");
		while(!exited()) {}
		log->notice("Server stopped.");
	//~ }
}
//~ void ServerStream::sendCommand(std::string command)
//~ {
	//~ log->debug("Connection::sendCommand");
	//~ if(!serverProcess->exited())
	//~ {
		//~ log->info("Sending command \"" + command + "\" to "+ serverName);
		//~ command = command + "\n";
		//~ serverProcess->sputn(command.c_str(), command.size());
	//~ } else {
		//~ log->info("Server not running");
	//~ }
//~ }
}
//~ /usr/bin/java -Xmx2048M -Xms1024M -XX:ParallelGCThreads=1 -XX:+UseConcMarkSweepGC -XX:+CMSIncrementalPacing -XX:+AggressiveOpts -jar /home/mel/minecraft/minecraft_server.jar nogui




//~ fdbuf::fdbuf(int fd)
  //~ : fd_(-1) {
    //~ this->open(fd);
//~ }

//~ fdbuf::~fdbuf() {
    //~ this->close();
//~ }

//~ void fdbuf::open(int fd) {
    //~ this->close();
    //~ this->fd_ = fd;
    //~ this->setg(this->inbuf_, this->inbuf_, this->inbuf_);
    //~ this->setp(this->outbuf_, this->outbuf_ + bufsize - 1);
//~ }

//~ void fdbuf::close() {
    //~ if (!(this->fd_ < 0)) {
        //~ this->sync();
        //~ ::close(this->fd_);
    //~ }
//~ }

//~ int fdbuf::overflow(int c) {
    //~ if (!traits_type::eq_int_type(c, traits_type::eof())) {
        //~ *this->pptr() = traits_type::to_char_type(c);
        //~ this->pbump(1);
    //~ }
    //~ return this->sync() == -1
        //~ ? traits_type::eof()
        //~ : traits_type::not_eof(c);
//~ }

//~ int fdbuf::sync() {
    //~ if (this->pbase() != this->pptr()) {
        //~ std::streamsize size(this->pptr() - this->pbase());
        //~ std::streamsize done(::write(this->childProcessStdin[PIPE_WRITE], this->outbuf_, size));
        //~ // The code below assumes that it is success if the stream made
        //~ // some progress. Depending on the needs it may be more
        //~ // reasonable to consider it a success only if it managed to
        //~ // write the entire buffer and, e.g., loop a couple of times
        //~ // to try achieving this success.
        //~ if (0 < done) {
            //~ std::copy(this->pbase() + done, this->pptr(), this->pbase());
            //~ this->setp(this->pbase(), this->epptr());
            //~ this->pbump(size - done);
        //~ }
    //~ }
    //~ return this->pptr() != this->epptr()? 0: -1;
//~ }

//~ int fdbuf::underflow()
//~ {
    
//~ }
