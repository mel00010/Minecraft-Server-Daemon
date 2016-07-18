/*******************************************************************************
 *
 * Minecraft Server Daemon
 * CLI.cpp
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
#define KEY_TAB 9
#include <CLI.hpp>
#include <curses.h>
#include <ncurses.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <map>
#include <string>
#include <utility>
namespace CLI {
void cleanup() {
	delete MainWindow;
	endwin();
	refresh();
	exit(EXIT_SUCCESS);
}
void initColors() {
	init_pair(NORMAL, COLOR_WHITE, COLOR_BLACK);
	init_pair(HIGHLIGHT, COLOR_BLACK, COLOR_WHITE);
	init_pair(TITLE, COLOR_WHITE, COLOR_BLUE);
	init_pair(TITLE_HIGHLIGHT, COLOR_BLUE, COLOR_WHITE);
	init_pair(MAIN_TITLE, COLOR_WHITE, COLOR_BLUE);
	init_pair(MAIN_TITLE_HIGHLIGHT, COLOR_BLUE, COLOR_WHITE);
}
void createWindows() {
	int quarterScreen = (COLS - 1) / 4;
	int colsRemaining = COLS - (4 * quarterScreen);
	int centerPanelWidth = (2 * quarterScreen) + colsRemaining - 2;

	MainWindow->addSubWindow("TitleBar", 1, COLS, 0, 0);

	MainWindow->addSubWindow("BottomBar", 1, COLS, LINES - 1, 0);
	MainWindow->sub("BottomBar")->addSubWindow("MainMode", 1, quarterScreen, 0, 0);
	MainWindow->sub("BottomBar")->addSubWindow("PluginsMode", 1, quarterScreen, 0, quarterScreen);
	MainWindow->sub("BottomBar")->addSubWindow("ConfigurationMode", 1, quarterScreen, 0, 2 * quarterScreen);
	MainWindow->sub("BottomBar")->addSubWindow("ManagementMode", 1, quarterScreen + colsRemaining, 0, 3 * quarterScreen);
	switch (currentMode) {
		case MAIN:
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
			break;
		case PLUGINS:
			break;
		case CONFIGURATION:
			break;
		case MANAGEMENT:
			break;
	}
}
void writeText() {

	MainWindow->sub("TitleBar")->write("Minecraft Server Manager");
	MainWindow->sub("TitleBar")->setAttributes(A_BOLD, MAIN_TITLE);

	MainWindow->sub("BottomBar")->sub("MainMode")->write("Main");
	MainWindow->sub("BottomBar")->sub("MainMode")->setAttributes(A_BOLD, MAIN_TITLE);

	MainWindow->sub("BottomBar")->sub("PluginsMode")->write("Plugins");
	MainWindow->sub("BottomBar")->sub("PluginsMode")->setAttributes(A_BOLD, MAIN_TITLE);

	MainWindow->sub("BottomBar")->sub("ConfigurationMode")->write("Configuration");
	MainWindow->sub("BottomBar")->sub("ConfigurationMode")->setAttributes(A_BOLD, MAIN_TITLE);

	MainWindow->sub("BottomBar")->sub("ManagementMode")->write("Management");
	MainWindow->sub("BottomBar")->sub("ManagementMode")->setAttributes(A_BOLD, MAIN_TITLE);

	switch (currentMode) {
		case MAIN:
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
			break;
		case PLUGINS:
			break;
		case CONFIGURATION:
			break;
		case MANAGEMENT:
			break;
	}
	wrefresh(MainWindow->win());
}
void redraw() {
	clear();
	MainWindow->deleteAllSubWindows();
	createWindows();
	writeText();
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
	refresh();
}
void quitPopup() {
	WINDOW* popupWin = newwin(5, 31, (LINES / 2) - 5, (COLS / 2) - 15);
	werase(popupWin);
	wmove(popupWin, 1, 2);
	waddstr(popupWin, "Do you really want to quit?");
	for (int i = 0; i != 4; i++) {
		mvwchgat(popupWin, i, 0, -1, A_REVERSE, NORMAL, NULL);
	}
	wborder(popupWin, ACS_VLINE | A_REVERSE | COLOR_PAIR(NORMAL), ACS_VLINE | A_REVERSE | COLOR_PAIR(NORMAL),
	ACS_HLINE | A_REVERSE | COLOR_PAIR(NORMAL), ACS_HLINE | A_REVERSE | COLOR_PAIR(NORMAL),
	ACS_ULCORNER | A_REVERSE | COLOR_PAIR(NORMAL), ACS_URCORNER | A_REVERSE | COLOR_PAIR(NORMAL),
	ACS_LLCORNER | A_REVERSE | COLOR_PAIR(NORMAL), ACS_LRCORNER | A_REVERSE | COLOR_PAIR(NORMAL));
	WINDOW* yesWin = derwin(popupWin, 1, 7, 3, 7);
	WINDOW* noWin = derwin(popupWin, 1, 6, 3, 18);
	waddstr(yesWin, "[ Yes ]");
	waddstr(noWin, "[ No ]");
	mvwchgat(noWin, 0, 0, 6, A_NORMAL, NORMAL, NULL);
	mvwchgat(yesWin, 0, 0, 7, A_REVERSE, NORMAL, NULL);
	wrefresh(popupWin);
	bool answer = true;
	int ch;
	for (;;) {
		if ((ch = getch()) == ERR) {
			nanosleep(sleeptime, NULL);
		} else {
			if ((ch == 'y') || (ch == 'Y') || (ch == 'q') || (ch == 'Q')) {
				delwin(yesWin);
				delwin(noWin);
				delwin(popupWin);
				cleanup();
			} else if ((ch == 'n') || (ch == 'n')) {
				delwin(yesWin);
				delwin(noWin);
				delwin(popupWin);
				redraw();
				break;
			} else if ((ch == KEY_LEFT) || (ch == KEY_RIGHT)) {
				if (answer == true) {
					answer = false;
					mvwchgat(noWin, 0, 0, 6, A_REVERSE, NORMAL, NULL);
					mvwchgat(yesWin, 0, 0, 7, A_NORMAL, NORMAL, NULL);
					wrefresh(noWin);
					wrefresh(yesWin);
				} else {
					answer = true;
					mvwchgat(noWin, 0, 0, 6, A_NORMAL, NORMAL, NULL);
					mvwchgat(yesWin, 0, 0, 7, A_REVERSE, NORMAL, NULL);
					wrefresh(noWin);
					wrefresh(yesWin);
				}
			} else if (ch == '\n') {
				if (answer == false) {
					delwin(yesWin);
					delwin(noWin);
					delwin(popupWin);
					cleanup();
				} else {
					delwin(yesWin);
					delwin(noWin);
					delwin(popupWin);
					redraw();
					break;
				}
			}
		}
	}
}
void drawScreen() {
	//	mvprintw(1, 0, "COLS = %d, LINES = %d", COLS, LINES);
	createWindows();
	writeText();
	MainWindow->sub("BottomBar")->sub("MainMode")->setAttributes(A_BOLD | A_REVERSE, MAIN_TITLE, true);
	wrefresh(MainWindow->sub("BottomBar")->win());
	refresh();

	/*  Loop until user hits 'q' to quit  */
	int ch;
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
							MainWindow->sub("BottomBar")->setAttributes(A_BOLD | A_NORMAL, MAIN_TITLE, true);
							switch (currentMode) {
								case MAIN:
									MainWindow->sub("BottomBar")->sub("MainMode")->setAttributes(A_BOLD | A_REVERSE, MAIN_TITLE, true);
									redraw();
									break;
								case PLUGINS:
									MainWindow->sub("BottomBar")->sub("PluginsMode")->setAttributes(A_BOLD | A_REVERSE, MAIN_TITLE, true);
									redraw();
									break;
								case CONFIGURATION:
									MainWindow->sub("BottomBar")->sub("ConfigurationMode")->setAttributes(A_BOLD | A_REVERSE, MAIN_TITLE, true);
									redraw();
									break;
								case MANAGEMENT:
									MainWindow->sub("BottomBar")->sub("ManagementMode")->setAttributes(A_BOLD | A_REVERSE, MAIN_TITLE, true);
									redraw();
									break;
							}
							wrefresh(MainWindow->sub("BottomBar")->win());
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
							MainWindow->sub("BottomBar")->setAttributes(A_BOLD | A_NORMAL, MAIN_TITLE, true);
							switch (currentMode) {
								case MAIN:
									MainWindow->sub("BottomBar")->sub("MainMode")->setAttributes(A_BOLD | A_REVERSE, MAIN_TITLE, true);
									redraw();
									break;
								case PLUGINS:
									MainWindow->sub("BottomBar")->sub("PluginsMode")->setAttributes(A_BOLD | A_REVERSE, MAIN_TITLE, true);
									redraw();
									break;
								case CONFIGURATION:
									MainWindow->sub("BottomBar")->sub("ConfigurationMode")->setAttributes(A_BOLD | A_REVERSE, MAIN_TITLE, true);
									redraw();
									break;
								case MANAGEMENT:
									MainWindow->sub("BottomBar")->sub("ManagementMode")->setAttributes(A_BOLD | A_REVERSE, MAIN_TITLE, true);
									redraw();
									break;
							}
							wrefresh(MainWindow->sub("BottomBar")->win());
						}
					}
					break;
				case KEY_TAB:
					//					MainWindow->write("KEY_TAB");
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
void sigintHandler(int sig)
{
	cleanup();
}
void runCLI() {
	signal(SIGINT, sigintHandler);

	/*  Initialize ncurses  */
	//	WINDOW* mainwin;
	if ((MainWindow = new WindowWrapper(initscr())) == NULL) {
		std::cerr << "Error initializing ncurses." << std::endl;
		exit(EXIT_FAILURE);
	}
	/*  Switch of echoing and enable keypad (for arrow keys)  */
	cbreak();
	noecho();
	nodelay(MainWindow->win(), TRUE);
	keypad(MainWindow->win(), TRUE);
	//	nonl();
	intrflush(MainWindow->win(), FALSE);
	start_color();
	initColors();
	drawScreen();
}
}
