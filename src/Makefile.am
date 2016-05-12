bin_PROGRAMS = minecraftd minecraft
minecraftd_SOURCES=main.cpp connection.cpp server.cpp parser.cpp jsoncpp.cpp mainloop.cpp setupservers.cpp
include_HEADERS = connection.h server.h parser.h mainloop.h setupservers.h
minecraftd_LDFLAGS = -pthread -levent -Lusr/log4cpp/lib -llog4cpp
minecraftd_CXXFLAGS= -Wall -Wextra -g -std=gnu++11 -I /usr/local/include
minecraft_SOURCES = client.cpp
minecraft_LDFLAGS =
minecraft_CXXFLAGS= -Wall -Wextra -g -std=gnu++11 -I /usr/local/include

