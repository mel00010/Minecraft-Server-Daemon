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

#include <MasterLoop.hpp>
#include <ConfigurationMode.hpp>
#include <MainMode.hpp>
#include <ManagementMode.hpp>
#include <PluginsMode.hpp>
#include <signal.h>
#include <stdlib.h>
#include <ctime>
#include <iostream>

namespace CLI {
const timespec* sleeptime = new timespec { 0, 4166666 };
WindowWrapper* MainWindow;
int height;
int width;
MODE currentMode = MAIN;
bool insideWindow = false;
bool bottomBarSelected = false;

void cleanup() {
	delete MainWindow;
	endwin();
	refresh();
	exit(EXIT_SUCCESS);
}
void initColors() {
	init_pair(NORMAL, COLOR_WHITE, COLOR_BLACK);
	init_pair(TITLE, COLOR_WHITE, COLOR_BLUE);
	init_pair(MAIN_TITLE, COLOR_WHITE, COLOR_BLUE);
}
void createWindows() {
	int quarterScreen = (COLS - 1) / 4;
	int colsRemaining = COLS - (4 * quarterScreen);

	MainWindow->addSubWindow("TitleBar", 1, COLS, 0, 0);

	MainWindow->addSubWindow("BottomBar", 1, COLS, LINES - 1, 0);
	MainWindow->sub("BottomBar")->addSubWindow("MainMode", 1, quarterScreen, 0, 0);
	MainWindow->sub("BottomBar")->addSubWindow("PluginsMode", 1, quarterScreen, 0, quarterScreen);
	MainWindow->sub("BottomBar")->addSubWindow("ConfigurationMode", 1, quarterScreen, 0, 2 * quarterScreen);
	MainWindow->sub("BottomBar")->addSubWindow("ManagementMode", 1, quarterScreen + colsRemaining, 0, 3 * quarterScreen);
	switch (currentMode) {
		case MAIN:
			createMainModeWindows();
			break;
		case PLUGINS:
			createPluginsModeWindows();
			break;
		case CONFIGURATION:
			createConfigurationModeWindows();
			break;
		case MANAGEMENT:
			createManagementModeWindows();
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
			writeMainModeText();
			break;
		case PLUGINS:
			writePluginsModeText();
			break;
		case CONFIGURATION:
			writeConfigurationModeText();
			break;
		case MANAGEMENT:
			writeManagementModeText();
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

void masterLoop() {
	createWindows();
	writeText();
	MainWindow->sub("BottomBar")->sub("MainMode")->setAttributes(A_BOLD | A_REVERSE, MAIN_TITLE, true);
	wrefresh(MainWindow->sub("BottomBar")->win());
	refresh();

	/*  Loop until user hits 'q' to quit  */
	for (;;) {
		nanosleep(sleeptime, NULL);
		switch (currentMode) {
			case MAIN:
				MainWindow->sub("BottomBar")->sub("MainMode")->setAttributes(A_BOLD | A_REVERSE, MAIN_TITLE, true);
				redraw();
				wrefresh(MainWindow->sub("BottomBar")->win());
				mainMode();
				break;
			case PLUGINS:
				MainWindow->sub("BottomBar")->sub("PluginsMode")->setAttributes(A_BOLD | A_REVERSE, MAIN_TITLE, true);
				redraw();
				wrefresh(MainWindow->sub("BottomBar")->win());
				pluginsMode();
				break;
			case CONFIGURATION:
				MainWindow->sub("BottomBar")->sub("ConfigurationMode")->setAttributes(A_BOLD | A_REVERSE, MAIN_TITLE, true);
				redraw();
				wrefresh(MainWindow->sub("BottomBar")->win());
				configurationMode();
				break;
			case MANAGEMENT:
				MainWindow->sub("BottomBar")->sub("ManagementMode")->setAttributes(A_BOLD | A_REVERSE, MAIN_TITLE, true);
				redraw();
				wrefresh(MainWindow->sub("BottomBar")->win());
				managementMode();
				break;
		}
		MainWindow->sub("BottomBar")->setAttributes(A_BOLD | A_NORMAL, MAIN_TITLE, true);

	}
}
void sigintHandler(__attribute__((unused)) int sig) {
	cleanup();
}
void runCLI() {
	signal(SIGINT, sigintHandler);

	/*  Initialize ncurses  */
	if ((MainWindow = new WindowWrapper(initscr())) == NULL) {
		std::cerr << "Error initializing ncurses." << std::endl;
		exit(EXIT_FAILURE);
	}
	/*  Switch of echoing and enable keypad (for arrow keys)  */
	cbreak();
	noecho();
	nodelay(MainWindow->win(), TRUE);
	keypad(MainWindow->win(), TRUE);
	intrflush(MainWindow->win(), FALSE);
	start_color();
	initColors();
	masterLoop();
}
}
