/*******************************************************************************
 *
 * Minecraft Server Daemon
 * CLI.hpp
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

#ifndef CLIENT_CLI_HPP_
#define CLIENT_CLI_HPP_

#include <curses.h>
#include <ncurses.h>
#include <map>
#include <string>
#include <utility>


void resizeHandler(int sig);
void sigintHandler(int sig);
namespace CLI {
enum COLORPAIRS {
	NORMAL, HIGHLIGHT, TITLE, TITLE_HIGHLIGHT, MAIN_TITLE, MAIN_TITLE_HIGHLIGHT
};
enum MODE {
	MAIN, PLUGINS, CONFIGURATION, MANAGEMENT
};
class WindowWrapper {
	protected:
		WINDOW* window;
		std::map<std::string, WindowWrapper*> subwindows;
		COLORPAIRS color = NORMAL;
		attr_t attributes = 0;
	public:
		WindowWrapper() {
			curs_set(0);
			window = nullptr;
		}
		WindowWrapper(WINDOW* parent, int height, int width, int ypos, int xpos) {
			curs_set(0);
			window = subwin(parent, height, width, ypos, xpos);
		}
		WindowWrapper(WindowWrapper& parent, int height, int width, int ypos, int xpos) {
			curs_set(0);
			window = subwin(parent.win(), height, width, ypos, xpos);
		}
		WindowWrapper(int height, int width, int ypos, int xpos) {
			curs_set(0);
			window = newwin(height, width, ypos, xpos);
		}
		WindowWrapper(WINDOW* _window) {
			curs_set(0);
			window = _window;
		}
		void setAttributes(attr_t attribute, COLORPAIRS pair, bool overwrite = false) {
			color = pair;
			if (overwrite == false) {
				if (attributes != attribute) {
					if ((attributes | attribute) != attributes) {
						attributes = attributes | attribute;
					}
				}
			} else {
				attributes = attribute;
			}
			for (int i = 0; i < LINES - 1; i++) {
				mvwchgat(window, i, 0, -1, attributes, pair, NULL);
			}
		}
		void boldAll(bool overwrite = false) {
			if (overwrite == false) {
				if (attributes != A_BOLD) {
					if ((attributes | A_BOLD) != attributes) {
						attributes = attributes | A_BOLD;
					}
				}
			} else {
				attributes = A_BOLD;
			}
			for (int i = 0; i < LINES - 1; i++) {
				mvwchgat(window, i, 0, -1, attributes, color, NULL);
			}
		}
		void unboldAll(bool overwrite = false) {
			if (overwrite == false) {
				if (attributes != A_NORMAL) {
					if ((attributes | A_NORMAL) != attributes) {
						attributes = attributes | A_NORMAL;
					}
				}
			} else {
				attributes = A_NORMAL;
			}
			for (int i = 0; i < LINES - 1; i++) {
				mvwchgat(window, i, 0, -1, attributes, color, NULL);
			}
		}
		void bold() {
			wattron(window, A_NORMAL);
		}
		void unbold() {
			wattroff(window, A_NORMAL);
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
		void addSubWindow(std::string name, int height, int width, int ypos, int xpos) {
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
MODE currentMode = MAIN;
int currentSelection = 0;
bool insideWindow = false;
int numberOfChoices = 4;
const timespec* sleeptime = new timespec { 0, 4166666 };
void initColors();
void runCLI();
void quitPopup();
void redraw();
void drawScreen();
void writeText();
void createWindows();
}
#endif /* CLIENT_CLI_HPP_ */
