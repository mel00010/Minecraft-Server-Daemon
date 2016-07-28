bin_PROGRAMS = minecraftd
minecraftd_SOURCES=daemon.cpp ConfigFileParser.cpp json/jsoncpp.cpp MainLoop.cpp Server.cpp SetupServers.cpp CreateSocket.cpp BukkitServer.cpp BungeeCordServer.cpp ForgeServer.cpp GenericServer.cpp SpigotServer.cpp VanillaServer.cpp 
if DEBUG
minecraftd_CXXFLAGS= -Wall -Wextra -g -std=gnu++11 -I /usr/local/include -D DEBUGGING=1
else
minecraftd_CXXFLAGS= -Wall -Wextra -g -std=gnu++11 -I /usr/local/include -D DEBUGGING=0
endif
include_HEADERS = ConfigFileParser.hpp MainLoop.hpp Server.hpp SetupServers.hpp CreateSocket.hpp ServerPropertyFileParser.hpp BukkitServer.hpp BungeeCordServer.hpp ForgeServer.hpp GenericServer.hpp SpigotServer.hpp VanillaServer.hpp
minecraftd_LDFLAGS = -pthread -levent -Lusr/log4cpp/lib -llog4cpp -lcurl

