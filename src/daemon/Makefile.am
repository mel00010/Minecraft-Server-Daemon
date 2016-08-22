bin_PROGRAMS = minecraftd
minecraftd_SOURCES=daemon.cpp ConfigFileParser.cpp IPC.cpp MainLoop.cpp Player.cpp Server.cpp SetupServers.cpp BukkitServer.cpp BungeeCordServer.cpp ForgeServer.cpp GenericServer.cpp SpigotServer.cpp VanillaServer.cpp 
minecraftd_CXXFLAGS= -Wall -Wextra -g -std=gnu++11 -I /usr/local/include
include_HEADERS = ConfigFileParser.hpp MainLoop.hpp Player.hpp Server.hpp IPC.hpp SetupServers.hpp ServerPropertyFileParser.hpp BukkitServer.hpp BungeeCordServer.hpp ForgeServer.hpp GenericServer.hpp SpigotServer.hpp VanillaServer.hpp
minecraftd_LDFLAGS = -pthread -levent -Lusr/log4cpp/lib -llog4cpp -lcurl -ljsoncpp

