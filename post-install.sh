#!/bin/bash
#
#  post-install.sh
#  
#  Copyright 2016 Mel McCalla <melmccalla@gmail.com>
#  
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#  MA 02110-1301, USA.
#  
#  
#  
groupadd minecraft
useradd -gminecraft -m minecraft
if ![ -d /etc/minecraft] ; then 
	mkdir /etc/minecraft
	chmod -R 755 /etc/minecraft
	chown -R minecraft:minecraft /etc/minecraft
fi
if ![ -d /var/log/minecraft] ; then
	mkdir /var/log/minecraft
	chmod -R 755 /var/log/minecraft
	chown -R minecraft:minecraft /var/log/minecraft
fi
if ![ -d /var/log/minecraft/servers] ; then 
	mkdir /var/log/minecraft/servers
	chmod -R 755 /var/log/minecraft/servers
	chown -R minecraft:minecraft /var/log/minecraft/servers
fi
if ![ -e /etc/minecraft/log4cpp.properties] ; then 
	cp log4cpp.properties /etc/minecraft/log4cpp.properties
	chmod 755 /etc/minecraft/log4cpp.properties
	chown minecraft:minecraft /etc/minecraft/log4cpp.properties
fi
if ![ -e /etc/minecraft/config.json] ; then 
	cp config.json /etc/minecraft/config.json
	chmod 55 /etc/minecraft/config.json
	chown minecraft:minecraft /etc/minecraft/config.json
fi
exit 0;
