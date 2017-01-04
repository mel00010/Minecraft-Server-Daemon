bin_PROGRAMS = minecraftd
noinst_PROGRAMS = test_ServerProtocol
test_ServerProtocol_SOURCES = Server/test_ServerProtocol.cpp Server/ServerProtocol.cpp
#check_PROGRAMS = test_

#test_audio_manager_SOURCES  = test_audio_manager.cc
#test_video_manager_SOURCES  = test_video_manager.cc

#TESTS = $(check_PROGRAMS)

minecraftd_SOURCES = \
	Daemon/daemon.cpp \
	Daemon/DaemonConfigFileParser.cpp \
	Daemon/MainLoop.cpp \
	\
	IPC/IPC.cpp \
	\
	Server/Player.cpp \
	Server/Server.cpp \
	Server/SetupServers.cpp \
	Server/BukkitServer.cpp \
	Server/BungeeCordServer.cpp \
	Server/ForgeServer.cpp \
	Server/GenericServer.cpp \
	Server/SpigotServer.cpp \
	Server/VanillaServer.cpp \
	Server/ServerProtocol.cpp \
	\
	PythonExtension/PythonFunctions.cpp \
	PythonExtension/ScriptHandler.cpp

minecraftd_CXXFLAGS = \
	-I/usr/include/python3.5m \
	-I/usr/include/python3.5m \
	-Wno-unused-result \
	-Wsign-compare \
	-g \
	-fdebug-prefix-map=/build/python3.5-SGM88Z/python3.5-3.5.2=. \
	-fstack-protector-strong \
	-Wformat \
	-Werror=format-security \
	-DNDEBUG \
	-g \
	-fwrapv \
	-O3 \
	-Wall \
	-Wextra \
	-std=gnu++11

include_HEADERS = \
	Daemon/DaemonConfigFileParser.hpp \
	Daemon/MainLoop.hpp \
	\
	IPC/IPC.hpp \
	IPC/InputMessage.hpp \
	IPC/OutputMessage.hpp \
	\
	Server/Server.hpp \
	Server/SetupServers.hpp \
	Server/ServerPropertyFileParser.hpp \
	Server/BukkitServer.hpp \
	Server/BungeeCordServer.hpp \
	Server/ForgeServer.hpp \
	Server/GenericServer.hpp \
	Server/SpigotServer.hpp \
	Server/VanillaServer.hpp \
	Server/Player.hpp \
	Server/ServerProtocol.hpp \
	\
	PythonExtension/ScriptHandler.hpp \
	PythonExtension/PythonFunctions.hpp \
	PythonExtension/GlobalVars.hpp

minecraftd_LDFLAGS = \
	-L/usr/lib/python3.5/config-3.5m-x86_64-linux-gnu \
	-L/usr/lib \
	-lpython3.5m \
	-lpthread \
	-ldl \
	-lutil \
	-lm \
	-Xlinker \
	-export-dynamic \
	-Wl,-O1 \
	-Wl,-Bsymbolic-functions \
	-levent \
	-Lusr/log4cpp/lib \
	-llog4cpp \
	-lcurl \
	-ljsoncpp
