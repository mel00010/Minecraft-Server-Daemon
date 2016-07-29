/*******************************************************************************
 *
 * Minecraft Server Daemon
 * QuitPrompt.cpp
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
#include <QuitPrompt.hpp>
#include <ctime>

namespace CLI {
void quitPrompt() {
	WINDOW* popupWin = newwin(5, 31, (LINES / 2) - 5, (COLS / 2) - 15);
	werase(popupWin);
	wmove(popupWin, 1, 2);
	waddstr(popupWin, "Do you really want to quit?");
	for (int i = 0; i != 4; i++) {
		mvwchgat(popupWin, i, 0, -1, A_REVERSE, NORMAL, NULL);
	}
	wborder(popupWin, ACS_VLINE | A_REVERSE | COLOR_PAIR(NORMAL), ACS_VLINE | A_REVERSE | COLOR_PAIR(NORMAL), ACS_HLINE | A_REVERSE | COLOR_PAIR(NORMAL),
	ACS_HLINE | A_REVERSE | COLOR_PAIR(NORMAL), ACS_ULCORNER | A_REVERSE | COLOR_PAIR(NORMAL), ACS_URCORNER | A_REVERSE | COLOR_PAIR(NORMAL),
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
			} else if (ch == KEY_RESIZE) {
				delwin(yesWin);
				delwin(noWin);
				delwin(popupWin);
				redraw();
				WINDOW* popupWin = newwin(5, 31, (LINES / 2) - 5, (COLS / 2) - 15);
				werase(popupWin);
				wmove(popupWin, 1, 2);
				waddstr(popupWin, "Do you really want to quit?");
				for (int i = 0; i != 4; i++) {
					mvwchgat(popupWin, i, 0, -1, A_REVERSE, NORMAL, NULL);
				}
				wborder(popupWin, ACS_VLINE | A_REVERSE | COLOR_PAIR(NORMAL), ACS_VLINE | A_REVERSE | COLOR_PAIR(NORMAL),
				ACS_HLINE | A_REVERSE | COLOR_PAIR(NORMAL), ACS_HLINE | A_REVERSE | COLOR_PAIR(NORMAL), ACS_ULCORNER | A_REVERSE | COLOR_PAIR(NORMAL),
				ACS_URCORNER | A_REVERSE | COLOR_PAIR(NORMAL), ACS_LLCORNER | A_REVERSE | COLOR_PAIR(NORMAL),
				ACS_LRCORNER | A_REVERSE | COLOR_PAIR(NORMAL));
				WINDOW* yesWin = derwin(popupWin, 1, 7, 3, 7);
				WINDOW* noWin = derwin(popupWin, 1, 6, 3, 18);
				waddstr(yesWin, "[ Yes ]");
				waddstr(noWin, "[ No ]");
				if (answer == true) {
					mvwchgat(noWin, 0, 0, 6, A_NORMAL, NORMAL, NULL);
					mvwchgat(yesWin, 0, 0, 7, A_REVERSE, NORMAL, NULL);
					wrefresh(noWin);
					wrefresh(yesWin);
				} else {
					mvwchgat(noWin, 0, 0, 6, A_REVERSE, NORMAL, NULL);
					mvwchgat(yesWin, 0, 0, 7, A_NORMAL, NORMAL, NULL);
					wrefresh(noWin);
					wrefresh(yesWin);
				}
				wrefresh(popupWin);
			}
		}
	}
}
}

