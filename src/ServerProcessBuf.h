/*
 * ServerProcessBuf.h
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
#ifndef SERVER_PROCESS_BUF_H
#define SERVER_PROCESS_BUF_H

#include <streambuf>
#include <iosfwd>
#include <cstdlib>
#include <vector>
#include "log4cpp/Category.hh"
namespace MinecraftServerService {
class ServerProcessBuf : public std::streambuf
{
    public:
        explicit ServerProcessBuf(std::string serverName, std::string serverPath, std::string serverJarName, std::string serverAccount,
			int maxHeapAlloc, int minHeapAlloc, int gcThreadCount,
			std::vector<std::string> javaArgs, std::vector<std::string> serverOptions);
		virtual ~ServerProcessBuf();
	    typedef std::streambuf::traits_type traits_type;
		enum PIPE_OPS 
		{
			PIPE_READ = 0,
			PIPE_WRITE = 1,
		};
		bool exited();
    protected:
		int overflow(int c);
		int underflow();
		int sync();
    private:
	    enum { bufsize = 1024 };
	    char outbuf_[bufsize];
	    char inbuf_[bufsize + 16 - sizeof(int)];
    	void startServer(
			std::string serverPath, std::string serverJarName, std::string serverAccount,
			int maxHeapAlloc, int minHeapAlloc, int gcThreadCount,
			std::vector<std::string> javaArgs, std::vector<std::string> serverOptions
		);
		void launchServerProcess(std::string serverPath, std::string serverJarName, std::string serverAccount,
			int maxHeapAlloc, int minHeapAlloc, int gcThreadCount,
			std::vector<std::string> javaArgs, std::vector<std::string> serverOptions);
		void getUIDAndGIDFromUsername(const char* user);
        //~ int sync();
		void stopServer();
        // copy ctor and assignment not implemented;
        // copying not allowed
        ServerProcessBuf(const ServerProcessBuf &);
        ServerProcessBuf &operator= (const ServerProcessBuf &);
        log4cpp::Category* log = nullptr;
        char currentCharacter;
        int server;
        int childProcessUID;
        int childProcessGID;
        int childProcessStdin[2];
		int childProcessStdout[2];
};
}
#endif /* SERVER_PROCESS_BUF_H */
