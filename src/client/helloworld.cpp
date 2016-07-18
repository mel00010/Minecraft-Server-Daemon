/*******************************************************************************
 *
 * Minecraft Server Daemon
 * helloworld.cpp
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
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <map>
#include <string>
#include <utility>
enum COLORPAIRS {
	NORMAL, HIGHLIGHT, TITLE, TITLE_HIGHLIGHT, MAIN_TITLE, MAIN_TITLE_HIGHLIGHT
};
class WindowWrapper {
	protected:
		WINDOW* window;
		std::map<std::string, WindowWrapper*> subwindows;
	public:
		WindowWrapper() {
			window = nullptr;
		}
		WindowWrapper(WINDOW* parent, int height, int width, int ypos,
				int xpos) {
			window = subwin(parent, height, width, ypos, xpos);
		}
		WindowWrapper(WindowWrapper& parent, int height, int width, int ypos, int xpos) {
			window = subwin(parent.win(), height, width, ypos, xpos);
		}
		WindowWrapper(int height, int width, int ypos, int xpos) {
			window = newwin(height, width, ypos, xpos);
		}
		WindowWrapper(WINDOW* _window) {
			window = _window;
		}
		void setColor(COLORPAIRS pair) {
			wbkgd(window, COLOR_PAIR(pair));
		}
		void bold() {
			wattron(window, A_BOLD);
		}
		void unbold() {
			wattroff(window, A_BOLD);
		}
		void write(std::string text) {
			waddstr(window, text.c_str());
		}
		void write(std::string text, int y, int x) {
			wmove(window, y, x);
			waddstr(window, text.c_str());
		}
		void write(const char* text) {
			waddstr(window, text);
		}
		void write(const char* text, int y, int x) {
			wmove(window, y, x);
			waddstr(window, text);
		}
		void addSubWindow(std::string name, int height, int width, int ypos,
				int xpos) {
			subwindows.insert(std::pair<std::string, WindowWrapper*>(name, new WindowWrapper(derwin(window, height, width, ypos, xpos))));
		}
		void deleteSubWindow(std::string subwindow) {
			if (subwindows.find(subwindow) != subwindows.end()) {
				delete subwindows.find(subwindow)->second;
				subwindows.erase(subwindow);
			}
		}
		void deleteAllSubWindows() {
			for (std::map<std::string, WindowWrapper*>::iterator it = subwindows.begin(); it != subwindows.end(); ++it) {
				delete it->second;
				subwindows.erase(it);
			}
		}
		void boxAllSubWindows() {
			for (std::map<std::string, WindowWrapper*>::iterator it = subwindows.begin(); it != subwindows.end(); ++it) {
				it->second->boxAllSubWindows();
				box(it->second->win(), 0, 0);
			}
		}
		inline WINDOW* win() {
			return getWindow();
		}
		WINDOW* getWindow() {
			return window;
		}
		inline WindowWrapper* sub(std::string subwindow) {
			return getSubWindow(subwindow);
		}
		WindowWrapper* getSubWindow(std::string subwindow) {
			if (subwindows.find(subwindow) != subwindows.end()) {
				return subwindows[subwindow];
			}
			return nullptr;
		}
		~WindowWrapper() {
			for (std::map<std::string, WindowWrapper*>::iterator it = subwindows.begin(); it != subwindows.end(); ++it) {
				delete it->second;
				subwindows.erase(it);
			}
			delwin(window);
		}
};
int height, width;
WindowWrapper* MainWindow;

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
void writeText() {
	MainWindow->sub("TitleBar")->setColor(MAIN_TITLE);
	MainWindow->sub("TitleBar")->bold();
	MainWindow->sub("TitleBar")->write("Minecraft Server Manager");

	MainWindow->sub("BottomBar")->setColor(MAIN_TITLE);
	MainWindow->sub("BottomBar")->bold();
	MainWindow->sub("BottomBar")->write("Bottom Bar");

	MainWindow->sub("LeftBorder")->setColor(NORMAL);
	MainWindow->sub("LeftBorder")->bold();
	mvwvline(MainWindow->sub("LeftBorder")->win(), 0, 0, ACS_VLINE, LINES);

	MainWindow->sub("RightBorder")->setColor(NORMAL);
	MainWindow->sub("RightBorder")->bold();
	mvwvline(MainWindow->sub("RightBorder")->win(), 0, 0, ACS_VLINE, LINES);

	MainWindow->sub("LeftPanel")->sub("ServerListTitle")->setColor(NORMAL);
	MainWindow->sub("LeftPanel")->sub("ServerListTitle")->bold();
	MainWindow->sub("LeftPanel")->sub("ServerListTitle")->write("Server List");

	MainWindow->sub("LeftPanel")->sub("ServerList")->setColor(NORMAL);

	MainWindow->sub("LeftPanel")->sub("ServerListTotal")->setColor(NORMAL);
	MainWindow->sub("LeftPanel")->sub("ServerListTotal")->write("Server List Total");

	MainWindow->sub("CenterPanel")->sub("ServerOutputTitle")->setColor(NORMAL);
	MainWindow->sub("CenterPanel")->sub("ServerOutputTitle")->bold();
	MainWindow->sub("CenterPanel")->sub("ServerOutputTitle")->write("Server Output");

	MainWindow->sub("CenterPanel")->sub("ServerOutput")->setColor(NORMAL);

	MainWindow->sub("CenterPanel")->sub("CommandPrompt")->setColor(NORMAL);

	MainWindow->sub("CenterPanel")->sub("CommandPrompt")->write("Command Prompt");

	MainWindow->sub("RightPanel")->sub("PlayerListTitle")->setColor(NORMAL);
	MainWindow->sub("RightPanel")->sub("PlayerListTitle")->bold();
	MainWindow->sub("RightPanel")->sub("PlayerListTitle")->write("Player List");

	MainWindow->sub("RightPanel")->sub("PlayerList")->setColor(NORMAL);

	MainWindow->sub("RightPanel")->sub("PlayerListTotal")->setColor(NORMAL);
	MainWindow->sub("RightPanel")->sub("PlayerListTotal")->write("Player List Total");

	wrefresh(MainWindow->win());
}
void drawScreen() {
	//	mvprintw(1, 0, "COLS = %d, LINES = %d", COLS, LINES);
	createWindows();
	writeText();
	//	MainWindow->boxAllSubWindows();
	refresh();

	/*  Loop until user hits 'q' to quit  */
	timespec* sleeptime = new timespec{0,4166666};
	int ch;
	for (;;) {
		if ((ch = getch()) == ERR) {
			nanosleep(sleeptime, NULL);
		} else {
			switch (ch) {
				case KEY_RESIZE:
					clear();
					MainWindow->deleteAllSubWindows();
					createWindows();
					writeText();
					//					MainWindow->boxAllSubWindows();
					refresh();
					break;
				case 'q':
					cleanup();
					break;
			}
		}
	}
}
void sigintHandler(int sig)
{
	cleanup();
}
int main(void) {
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
	nonl();
	intrflush(MainWindow->win(), FALSE);
	start_color();
	initColors();
	drawScreen();
	return 1;
}



