bin_PROGRAMS = minecraftd minecraft
if DEBUG
minecraftd_SOURCES=main.cpp parser.cpp jsoncpp.cpp mainLoop.cpp server.cpp setupServers.cpp createSocket.cpp vanillaServer.cpp bukkitServer.cpp bungeecordServer.cpp genericServer.cpp spigotServer.cpp  
minecraftd_CXXFLAGS= -Wall -Wextra -g -std=gnu++11 -I /usr/local/include -D DEBUGGING=1
else
minecraftd_SOURCES=main.cpp parser.cpp jsoncpp.cpp mainLoop.cpp setupServers.cpp createSocket.cpp vanillaServer.cpp bukkitServer.cpp bungeecordServer.cpp genericServer.cpp spigotServer.cpp  
minecraftd_CXXFLAGS= -Wall -Wextra -g -std=gnu++11 -I /usr/local/include -D DEBUGGING=0
endif
include_HEADERS = server.h parser.h mainLoop.h setupServers.h createSocket.h vanillaServer.h bukkitServer.h bungeecordServer.h genericServer.h spigotServer.h  
minecraftd_LDFLAGS = -pthread -levent -Lusr/log4cpp/lib -llog4cpp
minecraft_SOURCES = client.cpp
minecraft_LDFLAGS =
minecraft_CXXFLAGS= -Wall -Wextra -g -std=gnu++11 -I /usr/local/include

