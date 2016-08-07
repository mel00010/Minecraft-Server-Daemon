bin_PROGRAMS = minecraft
include_HEADERS = ParseOptions.hpp Socket.hpp Help.hpp MasterLoop.hpp CLI.hpp MainMode.hpp PluginsMode.hpp ConfigurationMode.hpp ManagementMode.hpp QuitPrompt.hpp WindowWrapper.hpp
minecraft_SOURCES = client.cpp ParseOptions.cpp Socket.cpp Help.cpp MasterLoop.cpp MainMode.cpp PluginsMode.cpp ConfigurationMode.cpp ManagementMode.cpp QuitPrompt.cpp
minecraft_LDFLAGS = -pthread -lncurses -Lusr/log4cpp/lib -llog4cpp
minecraft_CXXFLAGS= -Wall -Wextra -g -std=gnu++11 -I /usr/local/include