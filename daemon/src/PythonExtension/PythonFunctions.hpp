/*******************************************************************************
 *
 * MinecraftServerDaemon
 * PythonFunctions.hpp
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

#ifndef DAEMON_PYTHONFUNCTIONS_HPP_
#define DAEMON_PYTHONFUNCTIONS_HPP_

#include <Python.h>
#include <structmember.h>

extern "C" {
	PyMODINIT_FUNC PyInit_minecraftServerManager(void);
	static PyObject* minecraft_log(PyObject* self, PyObject* args);
	static PyObject* minecraft_test(PyObject* self, PyObject* args);
	static PyObject* minecraft_ListServers(PyObject* self, PyObject* args);
	static PyObject* minecraft_StartAll(PyObject* self, PyObject* args);
	static PyObject* minecraft_StartServer(PyObject* self, PyObject* args);
	static PyObject* minecraft_StopAll(PyObject* self, PyObject* args);
	static PyObject* minecraft_StopServer(PyObject* self, PyObject* args);
	static PyObject* minecraft_RestartAll(PyObject* self, PyObject* args);
	static PyObject* minecraft_RestartServer(PyObject* self, PyObject* args);
	static PyObject* minecraft_ServerStatus(PyObject* self, PyObject* args);
	static PyObject* minecraft_SendCommand(PyObject* self, PyObject* args);
	static PyObject* minecraft_ListOnlinePlayers(PyObject* self, PyObject* args);
	static PyObject* minecraft_ListOnlinePlayersFiltered(PyObject* self, PyObject* args);
	static PyObject* minecraft_UpdateAll(PyObject* self, PyObject* args);
	static PyObject* minecraft_UpdateServer(PyObject* self, PyObject* args);
	static PyObject* minecraft_BackupAll(PyObject* self, PyObject* args);
	static PyObject* minecraft_BackupServer(PyObject* self, PyObject* args);
	static PyObject* minecraft_OpPlayer(PyObject* self, PyObject* args);
	static PyObject* minecraft_DeopPlayer(PyObject* self, PyObject* args);
	static PyObject* minecraft_KickPlayer(PyObject* self, PyObject* args);
	static PyObject* minecraft_KickPlayerReason(PyObject* self, PyObject* args);
	static PyObject* minecraft_BanPlayer(PyObject* self, PyObject* args);
	static PyObject* minecraft_BanPlayerReason(PyObject* self, PyObject* args);
	static PyObject* minecraft_PardonPlayer(PyObject* self, PyObject* args);
}

static PyObject *ManagerError;

static PyMethodDef ManagerMethods[] =
{
	{
		"log",
		minecraft_log,
		METH_VARARGS,
		"Log a message."
	},
	{
		"test",
		minecraft_test,
		METH_VARARGS,
		"Test."
	},
	{
		"listServers",
		minecraft_ListServers,
		METH_VARARGS,
		"List all servers."
	},
	{
		"startAll",
		minecraft_StartAll,
		METH_VARARGS,
		"Start all servers."
	},
	{
		"startServer",
		minecraft_StartServer,
		METH_VARARGS,
		"Start a server."
	},
	{
		"stopAll",
		minecraft_StopAll,
		METH_VARARGS,
		"Stop all servers."
	},
	{
		"stopServer",
		minecraft_StopServer,
		METH_VARARGS,
		"Stop a server."
	},
	{
		"restartAll",
		minecraft_RestartAll,
		METH_VARARGS,
		"Restart all servers."
	},
	{
		"restartServer",
		minecraft_RestartServer,
		METH_VARARGS,
		"Restart a server."
	},
	{
		"serverStatus",
		minecraft_ServerStatus,
		METH_VARARGS,
		"Get the status of a server."
	},
	{
		"sendCommand",
		minecraft_SendCommand,
		METH_VARARGS,
		"Send a command to a server."
	},
	{
		"listOnlinePlayers",
		minecraft_ListOnlinePlayers,
		METH_VARARGS,
		"List the players on a server."
	},
	{
		"listOnlinePlayersFiltered",
		minecraft_ListOnlinePlayersFiltered,
		METH_VARARGS,
		"Check if a player is on a server."
	},
	{
		"updateAll",
		minecraft_UpdateAll,
		METH_VARARGS,
		"Update all servers to a specified version."
	},
	{
		"updateServer",
		minecraft_UpdateServer,
		METH_VARARGS,
		"Update a server to a specified version."
	},
	{
		"backupAll",
		minecraft_BackupAll,
		METH_VARARGS,
		"Back up all servers."
	},
	{
		"backupServer",
		minecraft_BackupServer,
		METH_VARARGS,
		"Back up a server."
	},
	{
		"opPlayer",
		minecraft_OpPlayer,
		METH_VARARGS,
		"Op a player on a server."
	},
	{
		"deopPlayer",
		minecraft_DeopPlayer,
		METH_VARARGS,
		"Deop a player on a server."
	},
	{
		"kickPlayer",
		minecraft_KickPlayer,
		METH_VARARGS,
		"Kick a player from a server."
	},
	{
		"kickPlayerReason",
		minecraft_KickPlayerReason,
		METH_VARARGS,
		"Kick a player from a server with a reason."
	},
	{
		"banPlayer",
		minecraft_BanPlayer,
		METH_VARARGS,
		"Ban a player from a server."
	},
	{
		"banPlayerReason",
		minecraft_BanPlayerReason,
		METH_VARARGS,
		"Ban a player from a server with a reason."
	},
	{
		"pardonPlayer",
		minecraft_PardonPlayer,
		METH_VARARGS,
		"Pardon a player from a ban."
	},
	{
		NULL,
		NULL,
		0,
		NULL
	}
};

static struct PyModuleDef managerModule = {
	PyModuleDef_HEAD_INIT,
	"minecraftServerManager",
	NULL,
	-1,
	ManagerMethods
};

#endif /* DAEMON_PYTHONFUNCTIONS_HPP_ */
