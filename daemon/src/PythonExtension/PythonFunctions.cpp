/*******************************************************************************
 *
 * MinecraftServerDaemon
 * PythonFunctions.cpp
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
#include <log4cpp/Category.hh>
#include <PythonExtension/GlobalVars.hpp>
#include <PythonExtension/PythonFunctions.hpp>
#include <vector>
#include <string>

PyMODINIT_FUNC PyInit_minecraftServerManager(void) {
	PyObject *m;

	m = PyModule_Create(&managerModule);
	if (m == NULL) {
		return NULL;
	}
	ManagerError = PyErr_NewException("minecraftServerManager.error", NULL, NULL);
	Py_INCREF(ManagerError);
	PyModule_AddObject(m, "error", ManagerError);
	return m;
}
static PyObject* minecraft_log(__attribute__((unused))      PyObject* self, PyObject* args) {
	const char *message;

	if (!PyArg_ParseTuple(args, "s", &message))
		return NULL;
	logPython->info(message);

	return Py_True;
}
static PyObject* minecraft_test(__attribute__((unused))                PyObject* self, __attribute__((unused))                PyObject* args) {
	logPython->debug("Call from python script.");
	return Py_True;
}
static PyObject* minecraft_ListServers(__attribute__((unused))                PyObject* self, __attribute__((unused))                PyObject* args) {
//	logPython->debug("minecraft_ListServers");
	std::vector<std::string> serverNames;
	PyObject* serverList = PyTuple_New(serversPython->size());
	for (std::vector<MinecraftServerDaemon::Server*>::iterator i = serversPython->begin(); i != serversPython->end(); i++) {
		serverNames.push_back((*i)->getServerName());
	}
	for (std::vector<std::string>::iterator i = serverNames.begin(); i != serverNames.end(); i++) {
		PyTuple_SET_ITEM(serverList, std::distance(serverNames.begin(), i), PyUnicode_FromString((*i).c_str()));
	}
	return serverList;
}
static PyObject* minecraft_StartAll(__attribute__((unused))                PyObject* self, __attribute__((unused))                PyObject* args) {

//	logPython->debug("minecraft_StartAll");
	for (std::vector<MinecraftServerDaemon::Server*>::iterator i = serversPython->begin(); i != serversPython->end(); i++) {
		(*i)->startServer();
	}
	return Py_True;
}
static PyObject* minecraft_StartServer(__attribute__((unused))                PyObject* self, PyObject* args) {
	const char *server;

	if (!PyArg_ParseTuple(args, "s", &server))
		return NULL;
//	logPython->debug("minecraft_StartServer");
//	logPython->debug(server);
	for (std::vector<MinecraftServerDaemon::Server*>::iterator i = serversPython->begin(); i != serversPython->end(); i++) {
		if ((*i)->getServerName() == server) {
			(*i)->startServer();
			break;
		}
	}
	return Py_True;
}
static PyObject* minecraft_StopAll(__attribute__((unused))                PyObject* self, __attribute__((unused))                PyObject* args) {

//	logPython->debug("minecraft_StopAll");
	for (std::vector<MinecraftServerDaemon::Server*>::iterator i = serversPython->begin(); i != serversPython->end(); i++) {
		(*i)->stopServer();
	}
	return Py_True;

}
static PyObject* minecraft_StopServer(__attribute__((unused))      PyObject* self, PyObject* args) {
	const char *server;

	if (!PyArg_ParseTuple(args, "s", &server))
		return NULL;
//	logPython->debug("minecraft_StopServer");
//	logPython->debug(server);
	for (std::vector<MinecraftServerDaemon::Server*>::iterator i = serversPython->begin(); i != serversPython->end(); i++) {
//		logPython->debug((*i)->getServerName());
		if ((*i)->getServerName() == server) {
//			logPython->debug("Found server");
			(*i)->stopServer();

			break;
		}
	}
	return Py_True;
}
static PyObject* minecraft_RestartAll(__attribute__((unused))                PyObject* self, __attribute__((unused))                PyObject* args) {
//	logPython->debug("minecraft_RestartAll");
	for (std::vector<MinecraftServerDaemon::Server*>::iterator i = serversPython->begin(); i != serversPython->end(); i++) {
		(*i)->restartServer();
	}
	return Py_True;
}
static PyObject* minecraft_RestartServer(__attribute__((unused))      PyObject* self, PyObject* args) {
	const char *server;

	if (!PyArg_ParseTuple(args, "s", &server))
		return NULL;
//	logPython->debug("minecraft_RestartServer");
//	logPython->debug(server);
	for (std::vector<MinecraftServerDaemon::Server*>::iterator i = serversPython->begin(); i != serversPython->end(); i++) {
//		logPython->debug((*i)->getServerName());
		if ((*i)->getServerName() == server) {
			(*i)->restartServer();

			break;
		}
	}
	return Py_True;
}
static PyObject* minecraft_ServerStatus(__attribute__((unused))      PyObject* self, PyObject* args) {
	const char *server;

	if (!PyArg_ParseTuple(args, "s", &server))
		return NULL;
//	logPython->debug("minecraft_ServerStatus");
//	logPython->debug(server);
	for (std::vector<MinecraftServerDaemon::Server*>::iterator i = serversPython->begin(); i != serversPython->end(); i++) {
//		logPython->debug((*i)->getServerName());
		if ((*i)->getServerName() == server) {
			logPython->warn("Status not implemented yet");
			break;
		}
	}
	return Py_True;
}
static PyObject* minecraft_SendCommand(__attribute__((unused))      PyObject* self, PyObject* args) {
	const char *server;
	const char *command;

	if (!PyArg_ParseTuple(args, "ss", &server, &command))
		return NULL;
//	logPython->debug("minecraft_SendCommand");
//	logPython->debug(server);
//	logPython->debug(command);
	for (std::vector<MinecraftServerDaemon::Server*>::iterator i = serversPython->begin(); i != serversPython->end(); i++) {
		logPython->debug((*i)->getServerName());
		if ((*i)->getServerName() == server) {
//			logPython->debug("Found server");
			(*i)->sendCommand(command);

			break;
		}
	}
	return Py_True;
}
static PyObject* minecraft_ListOnlinePlayers(__attribute__((unused))      PyObject* self, PyObject* args) {
	const char *server;

	if (!PyArg_ParseTuple(args, "s", &server))
		return NULL;
//	logPython->debug("minecraft_ListOnlinePlayers");
//	logPython->debug(server);
	for (std::vector<MinecraftServerDaemon::Server*>::iterator i = serversPython->begin(); i != serversPython->end(); i++) {
//		logPython->debug((*i)->getServerName());
		if ((*i)->getServerName() == server) {
			(*i)->listOnlinePlayers();
			break;
		}
	}
	return Py_True;
}
static PyObject* minecraft_ListOnlinePlayersFiltered(__attribute__((unused))      PyObject* self, PyObject* args) {
	const char *server;
	const char *player;
	if (!PyArg_ParseTuple(args, "ss", &server, &player))
		return NULL;
//	logPython->debug("minecraft_ListOnlinePlayersFiltered");
//	logPython->debug(server);
//	logPython->debug(player);
	for (std::vector<MinecraftServerDaemon::Server*>::iterator i = serversPython->begin(); i != serversPython->end(); i++) {
//		logPython->debug((*i)->getServerName());
		if ((*i)->getServerName() == server) {
			(*i)->listOnlinePlayers(player);
			break;
		}
	}
	return Py_True;
}
static PyObject* minecraft_UpdateAll(__attribute__((unused))                PyObject* self, __attribute__((unused))                PyObject* args) {
	const char *version;

	if (!PyArg_ParseTuple(args, "s", &version))
		return NULL;
//	logPython->debug("minecraft_UpdateAll");
//	logPython->debug(version);
	for (std::vector<MinecraftServerDaemon::Server*>::iterator i = serversPython->begin(); i != serversPython->end(); i++) {
		if ((*i)->getServerType() == MinecraftServerDaemon::Server::ServerType::VANILLA) {
			(*i)->updateServer(version);

			break;
		}
	}
	return Py_True;
}
static PyObject* minecraft_UpdateServer(__attribute__((unused))      PyObject* self, PyObject* args) {
	const char *server;
	const char *version;
	if (!PyArg_ParseTuple(args, "ss", &server, &version))
		return NULL;
//	logPython->debug("minecraft_UpdateServer");
//	logPython->debug(server);
//	logPython->debug(version);
	for (std::vector<MinecraftServerDaemon::Server*>::iterator i = serversPython->begin(); i != serversPython->end(); i++) {
//		logPython->debug((*i)->getServerName());
		if ((*i)->getServerName() == server) {
			if ((*i)->getServerType() == MinecraftServerDaemon::Server::ServerType::VANILLA) {
				(*i)->updateServer(version);

				break;
			}
		}
	}
	return Py_True;
}
static PyObject* minecraft_BackupAll(__attribute__((unused))                PyObject* self, __attribute__((unused))                PyObject* args) {
//	logPython->debug("minecraft_BackupAll");
	for (std::vector<MinecraftServerDaemon::Server*>::iterator i = serversPython->begin(); i != serversPython->end(); i++) {
		(*i)->backupServer();
	}
	return Py_True;
}
static PyObject* minecraft_BackupServer(__attribute__((unused))      PyObject* self, PyObject* args) {
	const char *server;

	if (!PyArg_ParseTuple(args, "s", &server))
		return NULL;
//	logPython->debug("minecraft_BackupServer");
//	logPython->debug(server);
	for (std::vector<MinecraftServerDaemon::Server*>::iterator i = serversPython->begin(); i != serversPython->end(); i++) {
		if ((*i)->getServerName() == server) {
			(*i)->backupServer();

			break;
		}
	}
	return Py_True;
}
static PyObject* minecraft_OpPlayer(__attribute__((unused))      PyObject* self, PyObject* args) {
	const char *server;
	const char* player;
	if (!PyArg_ParseTuple(args, "ss", &server, &player))
		return NULL;
//	logPython->debug("minecraft_OpPlayer");
//	logPython->debug(server);
//	logPython->debug(player);
	for (std::vector<MinecraftServerDaemon::Server*>::iterator i = serversPython->begin(); i != serversPython->end(); i++) {
		if ((*i)->getServerName() == server) {
			for (std::vector<MinecraftServerDaemon::Player*>::iterator j = (*i)->getPlayerVector()->begin(); j != (*i)->getPlayerVector()->end(); j++) {
				if ((*j)->getPlayerName() == player) {
					(*j)->op();

					break;
				}
			}
			break;
		}
	}
	return Py_True;
}
static PyObject* minecraft_DeopPlayer(__attribute__((unused))      PyObject* self, PyObject* args) {
	const char *server;
	const char* player;
	if (!PyArg_ParseTuple(args, "ss", &server, &player))
		return NULL;
//	logPython->debug("minecraft_DeopPlayer");
//	logPython->debug(server);
//	logPython->debug(player);
	for (std::vector<MinecraftServerDaemon::Server*>::iterator i = serversPython->begin(); i != serversPython->end(); i++) {
		if ((*i)->getServerName() == server) {
			for (std::vector<MinecraftServerDaemon::Player*>::iterator j = (*i)->getPlayerVector()->begin(); j != (*i)->getPlayerVector()->end(); j++) {
				if ((*j)->getPlayerName() == player) {
					(*j)->deop();

					break;
				}
			}
			break;
		}
	}
	return Py_True;
}
static PyObject* minecraft_KickPlayer(__attribute__((unused))      PyObject* self, PyObject* args) {
	const char *server;
	const char* player;
	if (!PyArg_ParseTuple(args, "ss", &server, &player))
		return NULL;
//	logPython->debug("minecraft_KickPlayer");
//	logPython->debug(server);
//	logPython->debug(player);
	bool kickedPlayer = false;
	for (std::vector<MinecraftServerDaemon::Server*>::iterator i = serversPython->begin(); i != serversPython->end(); i++) {
		if ((*i)->getServerName() == server) {
			for (std::vector<MinecraftServerDaemon::Player*>::iterator j = (*i)->getPlayerVector()->begin(); j != (*i)->getPlayerVector()->end(); j++) {
				if ((*j)->getPlayerName() == player) {
//					logPython->debug((*j)->getPlayerName());
					(*j)->kick();
					kickedPlayer = true;
					break;
				}
			}
			if (kickedPlayer) {
				return Py_True;
			} else {
				return Py_False;
			}
			break;
		}

	}
	return Py_True;
}
static PyObject* minecraft_KickPlayerReason(__attribute__((unused))      PyObject* self, PyObject* args) {
	const char *server;
	const char* player;
	const char* reason;
	if (!PyArg_ParseTuple(args, "sss", &server, &player, &reason))
		return NULL;
//	logPython->debug("minecraft_KickPlayerReason");
//	logPython->debug(server);
//	logPython->debug(player);
//	logPython->debug(reason);
	for (std::vector<MinecraftServerDaemon::Server*>::iterator i = serversPython->begin(); i != serversPython->end(); i++) {
		if ((*i)->getServerName() == server) {
			for (std::vector<MinecraftServerDaemon::Player*>::iterator j = (*i)->getPlayerVector()->begin(); j != (*i)->getPlayerVector()->end(); j++) {
				if ((*j)->getPlayerName() == player) {
					(*j)->kick(reason);

					break;
				}
			}
			break;
		}
	}
	return Py_True;
}
static PyObject* minecraft_BanPlayer(__attribute__((unused))      PyObject* self, PyObject* args) {
	const char *server;
	const char* player;
	if (!PyArg_ParseTuple(args, "ss", &server, &player))
		return NULL;
//	logPython->debug("minecraft_BanPlayer");
//	logPython->debug(server);
//	logPython->debug(player);
	for (std::vector<MinecraftServerDaemon::Server*>::iterator i = serversPython->begin(); i != serversPython->end(); i++) {
		if ((*i)->getServerName() == server) {
			for (std::vector<MinecraftServerDaemon::Player*>::iterator j = (*i)->getPlayerVector()->begin(); j != (*i)->getPlayerVector()->end(); j++) {
				if ((*j)->getPlayerName() == player) {
					(*j)->ban();

					break;
				}
			}
			break;
		}
	}
	return Py_True;
}
static PyObject* minecraft_BanPlayerReason(__attribute__((unused))      PyObject* self, PyObject* args) {
	const char *server;
	const char* player;
	const char* reason;
	if (!PyArg_ParseTuple(args, "sss", &server, &player, &reason))
		return NULL;
//	logPython->debug("minecraft_BanPlayerReason");
//	logPython->debug(server);
//	logPython->debug(player);
//	logPython->debug(reason);
	for (std::vector<MinecraftServerDaemon::Server*>::iterator i = serversPython->begin(); i != serversPython->end(); i++) {
		if ((*i)->getServerName() == server) {
			for (std::vector<MinecraftServerDaemon::Player*>::iterator j = (*i)->getPlayerVector()->begin(); j != (*i)->getPlayerVector()->end(); j++) {
				if ((*j)->getPlayerName() == player) {
					(*j)->ban(reason);

					break;
				}
			}
			break;
		}
	}
	return Py_True;
}
static PyObject* minecraft_PardonPlayer(__attribute__((unused))      PyObject* self, PyObject* args) {
	const char *server;
	const char* player;
	if (!PyArg_ParseTuple(args, "ss", &server, &player))
		return NULL;
//	logPython->debug("minecraft_PardonPlayer");
//	logPython->debug(server);
//	logPython->debug(player);
	for (std::vector<MinecraftServerDaemon::Server*>::iterator i = serversPython->begin(); i != serversPython->end(); i++) {
		if ((*i)->getServerName() == server) {
			(*((*i)->getPlayerVector()->begin()))->pardon(player);

			break;
		}
	}
	return Py_True;
}
