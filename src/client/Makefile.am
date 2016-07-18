bin_PROGRAMS = minecraft test
include_HEADERS = ParseOptions.hpp Socket.hpp Help.cpp CLI.cpp
minecraft_SOURCES = client.cpp ParseOptions.cpp Socket.cpp Help.cpp CLI.cpp
minecraft_LDFLAGS = -pthread -lncurses -Lusr/log4cpp/lib -llog4cpp
test_SOURCES = helloworld.cpp
test_LDFLAGS = -lncurses
test_CXXFLAGS= -Wall -Wextra -g -std=gnu++11 -I /usr/local/include
minecraft_CXXFLAGS= -Wall -Wextra -g -std=gnu++11 -I /usr/local/include