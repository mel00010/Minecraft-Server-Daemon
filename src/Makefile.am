bin_PROGRAMS = minecraftd minecraft
minecraftd_SOURCES=main.cpp parser.cpp jsoncpp.cpp mainLoop.cpp setupServers.cpp createSocket.cpp vanillaServer.cpp bukkitServer.cpp bungeecordServer.cpp genericServer.cpp spigotServer.cpp  
include_HEADERS = ServerProcessBuf.h server.h parser.h mainLoop.h setupServers.h createSocket.h vanillaServer.h bukkitServer.h bungeecordServer.h genericServer.h spigotServer.h  
minecraftd_LDFLAGS = -pthread -levent -Lusr/log4cpp/lib -llog4cpp
minecraftd_CXXFLAGS= -Wall -Wextra -g -std=gnu++11 -I /usr/local/include
minecraft_SOURCES = client.cpp
minecraft_LDFLAGS =
minecraft_CXXFLAGS= -Wall -Wextra -g -std=gnu++11 -I /usr/local/include

