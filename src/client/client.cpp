/*******************************************************************************
 *
 * Minecraft Server Daemon
 * client.cpp
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

#include <Help.hpp>
#include <MasterLoop.hpp>
#include <ParseOptions.hpp>
#include <cstdlib>
#include <iostream>
#include <string>


int main(int argc, char *argv[]) {
	if (argc > 1) {
		std::string option(argv[1]);
		if (option == "--help") {
			help(argv[0]);
		} else if (option == "--gui") {
			CLI::runCLI();
			return 0;
		} else if (option == "startdaemon") {
			if (system("minecraftd") == 0) {
				std::cout << "Daemon started" << std::endl;
				return 0;
			} else {
				std::cout << "Daemon failed to start" << std::endl;
				return 1;
			}
		} else {
			parseOptions(argc, argv);
			return 0;
		}
	} else {
		help(argv[0]);
		return 1;
	}
}
