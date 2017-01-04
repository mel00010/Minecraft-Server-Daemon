/*******************************************************************************
 *
 * Minecraft Server Daemon
 * ForgeServer.hpp
 * Copyright (C) 2016  Mel McCalla <melmccalla@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 *
 *******************************************************************************/
#include <Python.h>
#include <PythonExtension/PythonFunctions.hpp>
#include <PythonExtension/ScriptHandler.hpp>
#include <string>
#include <iostream>
log4cpp::Category* logPython;
std::vector<MinecraftServerDaemon::Server*>* serversPython;
void scriptLauncher(std::string scriptPath, std::vector<MinecraftServerDaemon::Server*>* servers, log4cpp::Category* log) {
	logPython = log;
	serversPython = servers;
	FILE* scriptFile = fopen(scriptPath.c_str(), "r");
	PyRun_SimpleFile(scriptFile, scriptPath.c_str());
	fclose(scriptFile);
}
