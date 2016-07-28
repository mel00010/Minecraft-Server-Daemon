/*******************************************************************************
 *
 * Minecraft Server Daemon
 * PluginMode.cpp
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

#include <curses.h>
#include <CLI.hpp>
#include <MasterLoop.hpp>
#include <PluginsMode.hpp>
#include <QuitPrompt.hpp>
#include <ctime>

namespace CLI {
void pluginsMode() {
	int ch;
	int currentSelection = 4;
	bottomBarSelected = true;
	insideWindow = true;
	for (;;) {
		if ((ch = getch()) == ERR) {
			nanosleep(sleeptime, NULL);
		} else {
			switch (ch) {
				case KEY_RESIZE:
					redraw();
					break;
				case KEY_LEFT:
					if (insideWindow == true) {
						if (bottomBarSelected == true) {
							switch (currentMode) {
								case MAIN:
									currentMode = MANAGEMENT;
									break;
								case PLUGINS:
									currentMode = MAIN;
									break;
								case CONFIGURATION:
									currentMode = PLUGINS;
									break;
								case MANAGEMENT:
									currentMode = CONFIGURATION;
									break;
							}
							return;
						}
					}
					break;
				case KEY_RIGHT:
					if (insideWindow == true) {
						if (bottomBarSelected == true) {
							switch (currentMode) {
								case MAIN:
									currentMode = PLUGINS;
									break;
								case PLUGINS:
									currentMode = CONFIGURATION;
									break;
								case CONFIGURATION:
									currentMode = MANAGEMENT;
									break;
								case MANAGEMENT:
									currentMode = MAIN;
									break;
							}
							return;
						}
					}
					break;
				case 'q':
					if (insideWindow == true) {
						insideWindow = false;
						switch (currentSelection) {
							case 1:
								MainWindow->sub("LeftPanel")->sub("ServerListTitle")->setAttributes(A_BOLD | A_REVERSE, NORMAL, true);
								wrefresh(MainWindow->sub("LeftPanel")->sub("ServerListTitle")->win());
								break;
							case 2:
								MainWindow->sub("CenterPanel")->sub("ServerOutputTitle")->setAttributes(A_BOLD | A_REVERSE, NORMAL, true);
								wrefresh(MainWindow->sub("CenterPanel")->sub("ServerOutputTitle")->win());
								MainWindow->sub("CenterPanel")->sub("CommandPrompt")->setAttributes(A_NORMAL, NORMAL, true);
								wrefresh(MainWindow->sub("CenterPanel")->sub("CommandPrompt")->win());
								break;
							case 3:
								MainWindow->sub("RightPanel")->sub("PlayerListTitle")->setAttributes(A_BOLD | A_REVERSE, NORMAL, true);
								wrefresh(MainWindow->sub("RightPanel")->sub("PlayerListTitle")->win());
								break;
							case 4:
								MainWindow->sub("BottomBar")->setAttributes(A_BOLD | A_REVERSE, MAIN_TITLE, true);
								wrefresh(MainWindow->sub("BottomBar")->win());
								break;
						}
					} else {
						quitPrompt();
					}
					break;
				case '\n':
					insideWindow = true;
					switch (currentSelection) {
						case 4:
							MainWindow->sub("BottomBar")->setAttributes(A_BOLD | A_NORMAL, MAIN_TITLE, true);
							switch (currentMode) {
								case MAIN:
									MainWindow->sub("BottomBar")->sub("MainMode")->setAttributes(A_BOLD | A_REVERSE, MAIN_TITLE, true);
									break;
								case PLUGINS:
									MainWindow->sub("BottomBar")->sub("PluginsMode")->setAttributes(A_BOLD | A_REVERSE, MAIN_TITLE, true);
									break;
								case CONFIGURATION:
									MainWindow->sub("BottomBar")->sub("ConfigurationMode")->setAttributes(A_BOLD | A_REVERSE, MAIN_TITLE, true);
									break;
								case MANAGEMENT:
									MainWindow->sub("BottomBar")->sub("ManagementMode")->setAttributes(A_BOLD | A_REVERSE, MAIN_TITLE, true);
									break;
							}
							wrefresh(MainWindow->sub("BottomBar")->win());
							break;
					}
					break;
			}
		}
	}
}
void createPluginsModeWindows() {
	int quarterScreen = (COLS - 1) / 4;
	int colsRemaining = COLS - (4 * quarterScreen);
	int rightPanelWidth = (3 * quarterScreen) + colsRemaining - 2;
	MainWindow->addSubWindow("LeftPanel", LINES - 2, quarterScreen, 1, 0);
	MainWindow->sub("LeftPanel")->addSubWindow("ServerListTitle", 1, quarterScreen, 0, 0);
	MainWindow->sub("LeftPanel")->addSubWindow("ServerList", LINES - 4, quarterScreen, 1, 0);
	MainWindow->sub("LeftPanel")->addSubWindow("ServerListTotal", 1, quarterScreen, LINES - 3, 0);
	MainWindow->addSubWindow("LeftBorder", LINES - 2, 1, 1, quarterScreen);
	MainWindow->addSubWindow("RightPanel", LINES - 2, rightPanelWidth, 1, 0);
}
void writePluginsModeText() {
	mvwvline(MainWindow->sub("LeftBorder")->win(), 0, 0, ACS_VLINE | A_BOLD, LINES);

	MainWindow->sub("LeftPanel")->sub("ServerListTitle")->write("Server List");
	MainWindow->sub("LeftPanel")->sub("ServerListTitle")->setAttributes(A_BOLD, NORMAL);

	MainWindow->sub("LeftPanel")->sub("ServerList")->setAttributes(A_NORMAL, NORMAL);

	MainWindow->sub("LeftPanel")->sub("ServerListTotal")->write("Server List Total");
	MainWindow->sub("LeftPanel")->sub("ServerListTotal")->setAttributes(A_NORMAL, NORMAL);
}
}
