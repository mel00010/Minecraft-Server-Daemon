bin_PROGRAMS = minecraftd minecraft
minecraftd_SOURCES=main.cpp connection.cpp server.cpp parser.cpp jsoncpp.cpp
include_HEADERS = connection.h server.h parser.h
minecraftd_LDFLAGS = -pthread -Lusr/log4cpp/lib -llog4cpp
minecraftd_CXXFLAGS= -Wall -Wextra -g -std=gnu++11 -I /usr/local/include
minecraft_SOURCES = client.cpp
minecraft_LDFLAGS =
minecraft_CXXFLAGS= -Wall -Wextra -g -std=gnu++11 -I /usr/local/include

