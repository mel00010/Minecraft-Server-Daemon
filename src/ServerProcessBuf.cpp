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
