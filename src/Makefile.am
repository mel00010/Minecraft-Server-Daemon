bin_PROGRAMS = minecraftd minecraft
if DEBUG
minecraftd_SOURCES=main.cpp ConfigFileParser.cpp jsoncpp.cpp MainLoop.cpp Server.cpp SetupServers.cpp CreateSocket.cpp VanillaServer.cpp BukkitServer.cpp BungeeCordServer.cpp GenericServer.cpp SpigotServer.cpp ServerPropertyFileParser.cpp 
minecraftd_CXXFLAGS= -Wall -Wextra -g -std=gnu++11 -I /usr/local/include -D DEBUGGING=1
else
minecraftd_SOURCES=main.cpp ConfigFileParser.cpp jsoncpp.cpp MainLoop.cpp  SetupServers.cpp CreateSocket.cpp VanillaServer.cpp BukkitServer.cpp BungeeCordServer.cpp GenericServer.cpp SpigotServer.cpp  
minecraftd_CXXFLAGS= -Wall -Wextra -g -std=gnu++11 -I /usr/local/include -D DEBUGGING=0
endif
include_HEADERS = ConfigFileParser.hpp MainLoop.hpp Server.hpp SetupServers.hpp CreateSocket.hpp VanillaServer.hpp BukkitServer.hpp BungeeCordServer.hpp GenericServer.hpp SpigotServer.hpp ServerPropertyFileParser.hpp
minecraftd_LDFLAGS = -pthread -levent -Lusr/log4cpp/lib -llog4cpp -lcurl
minecraft_SOURCES = client.cpp
minecraft_LDFLAGS =
minecraft_CXXFLAGS= -Wall -Wextra -g -std=gnu++11 -I /usr/local/include

