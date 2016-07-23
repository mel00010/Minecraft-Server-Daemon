/*******************************************************************************
 *
 * Minecraft Server Daemon
 * MainMode.cpp
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
#include <MainMode.hpp>
#include <MasterLoop.hpp>
#include <QuitPrompt.hpp>
#include <ctime>

namespace CLI {
void mainMode() {
	int ch;
	int currentSelection;
	if (bottomBarSelected == true) {
		currentSelection = 4;
	} else {
		currentSelection = 0;
	}
	int numberOfChoices = 4;
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
						if (currentSelection == 4) {
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
						if (currentSelection == 4) {
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
				case KEY_TAB:
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
					}
					if (currentSelection == numberOfChoices) {
						currentSelection = 0;
					}
					currentSelection++;
					switch (currentSelection) {
						case 1:
							MainWindow->sub("LeftPanel")->sub("ServerListTitle")->setAttributes(A_BOLD | A_REVERSE, NORMAL, true);
							wrefresh(MainWindow->sub("LeftPanel")->sub("ServerListTitle")->win());
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
						case 2:
							MainWindow->sub("CenterPanel")->sub("ServerOutputTitle")->setAttributes(A_BOLD | A_REVERSE, NORMAL, true);
							wrefresh(MainWindow->sub("CenterPanel")->sub("ServerOutputTitle")->win());
							MainWindow->sub("LeftPanel")->sub("ServerListTitle")->setAttributes(A_BOLD | A_NORMAL, NORMAL, true);
							wrefresh(MainWindow->sub("LeftPanel")->sub("ServerListTitle")->win());
							break;
						case 3:
							MainWindow->sub("RightPanel")->sub("PlayerListTitle")->setAttributes(A_BOLD | A_REVERSE, NORMAL, true);
							wrefresh(MainWindow->sub("RightPanel")->sub("PlayerListTitle")->win());
							MainWindow->sub("CenterPanel")->sub("ServerOutputTitle")->setAttributes(A_BOLD | A_NORMAL, NORMAL, true);
							wrefresh(MainWindow->sub("CenterPanel")->sub("ServerOutputTitle")->win());
							break;
						case 4:
							MainWindow->sub("BottomBar")->setAttributes(A_BOLD | A_REVERSE, MAIN_TITLE, true);
							wrefresh(MainWindow->sub("BottomBar")->win());
							MainWindow->sub("RightPanel")->sub("PlayerListTitle")->setAttributes(A_BOLD | A_NORMAL, NORMAL, true);
							wrefresh(MainWindow->sub("RightPanel")->sub("PlayerListTitle")->win());
							break;
					}
					break;
				case '\n':
					insideWindow = true;
					switch (currentSelection) {
						case 1:
							//							MainWindow->sub("LeftPanel")->sub("ServerListTitle")->setAttributes(A_BOLD | A_REVERSE, NORMAL, true);
							//							wrefresh(MainWindow->sub("LeftPanel")->sub("ServerListTitle")->win());
							break;
						case 2:
							MainWindow->sub("CenterPanel")->sub("CommandPrompt")->setAttributes(A_BOLD | A_REVERSE, NORMAL, true);
							wrefresh(MainWindow->sub("CenterPanel")->sub("CommandPrompt")->win());
							break;
						case 3:
							//							MainWindow->sub("RightPanel")->sub("PlayerListTitle")->setAttributes(A_BOLD | A_REVERSE, NORMAL, true);
							//							wrefresh(MainWindow->sub("RightPanel")->sub("PlayerListTitle")->win());
							break;
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
						quitPopup();
					}
					break;
			}
		}
	}
}
void createMainModeWindows() {
	int quarterScreen = (COLS - 1) / 4;
	int colsRemaining = COLS - (4 * quarterScreen);
	int centerPanelWidth = (2 * quarterScreen) + colsRemaining - 2;
	MainWindow->addSubWindow("LeftPanel", LINES - 2, quarterScreen, 1, 0);
	MainWindow->sub("LeftPanel")->addSubWindow("ServerListTitle", 1, quarterScreen, 0, 0);
	MainWindow->sub("LeftPanel")->addSubWindow("ServerList", LINES - 4, quarterScreen, 1, 0);
	MainWindow->sub("LeftPanel")->addSubWindow("ServerListTotal", 1, quarterScreen, LINES - 3, 0);

	MainWindow->addSubWindow("LeftBorder", LINES - 2, 1, 1, quarterScreen);

	MainWindow->addSubWindow("CenterPanel", LINES - 2, centerPanelWidth, 1, quarterScreen + 1);
	MainWindow->sub("CenterPanel")->addSubWindow("ServerOutputTitle", 1, centerPanelWidth, 0, 0);
	MainWindow->sub("CenterPanel")->addSubWindow("ServerOutput", LINES - 4, centerPanelWidth, 1, 0);
	MainWindow->sub("CenterPanel")->addSubWindow("CommandPrompt", 1, centerPanelWidth, LINES - 3, 0);

	MainWindow->addSubWindow("RightBorder", LINES - 2, 1, 1, quarterScreen + centerPanelWidth + 1);

	MainWindow->addSubWindow("RightPanel", LINES - 2, quarterScreen, 1, (3 * quarterScreen) + colsRemaining);
	MainWindow->sub("RightPanel")->addSubWindow("PlayerListTitle", 1, quarterScreen, 0, 0);
	MainWindow->sub("RightPanel")->addSubWindow("PlayerList", LINES - 4, quarterScreen, 1, 0);
	MainWindow->sub("RightPanel")->addSubWindow("PlayerListTotal", 1, quarterScreen, LINES - 3, 0);
}
void writeMainModeText() {
	mvwvline(MainWindow->sub("LeftBorder")->win(), 0, 0, ACS_VLINE | A_BOLD, LINES);

	mvwvline(MainWindow->sub("RightBorder")->win(), 0, 0, ACS_VLINE | A_BOLD, LINES);

	MainWindow->sub("LeftPanel")->sub("ServerListTitle")->write("Server List");
	MainWindow->sub("LeftPanel")->sub("ServerListTitle")->setAttributes(A_BOLD, NORMAL);

	MainWindow->sub("LeftPanel")->sub("ServerList")->setAttributes(A_NORMAL, NORMAL);

	MainWindow->sub("LeftPanel")->sub("ServerListTotal")->write("Server List Total");
	MainWindow->sub("LeftPanel")->sub("ServerListTotal")->setAttributes(A_NORMAL, NORMAL);

	MainWindow->sub("CenterPanel")->sub("ServerOutputTitle")->write("Server Output");
	MainWindow->sub("CenterPanel")->sub("ServerOutputTitle")->setAttributes(A_BOLD, NORMAL);

	MainWindow->sub("CenterPanel")->sub("ServerOutput")->setAttributes(A_NORMAL, NORMAL);

	MainWindow->sub("CenterPanel")->sub("CommandPrompt")->setAttributes(A_NORMAL, NORMAL);

	MainWindow->sub("CenterPanel")->sub("CommandPrompt")->write("Command Prompt");

	MainWindow->sub("RightPanel")->sub("PlayerListTitle")->write("Player List");
	MainWindow->sub("RightPanel")->sub("PlayerListTitle")->setAttributes(A_BOLD, NORMAL);

	MainWindow->sub("RightPanel")->sub("PlayerList")->setAttributes(A_NORMAL, NORMAL);

	MainWindow->sub("RightPanel")->sub("PlayerListTotal")->write("Player List Total");
	MainWindow->sub("RightPanel")->sub("PlayerListTotal")->setAttributes(A_NORMAL, NORMAL);
}
}



