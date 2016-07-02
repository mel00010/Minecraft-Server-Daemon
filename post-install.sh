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
if [ -d "/etc/minecraft" ]
then 
	echo "Directory /etc/minecraft exists.  Not creating /etc/minecraft."
else
	echo "Creating directory /etc/minecraft"
	mkdir /etc/minecraft
	chmod -R 755 /etc/minecraft
	chown -R minecraft:minecraft /etc/minecraft
fi
if [ -d "/var/log/minecraft" ]
then
	echo "Directory /var/log/minecraft exists.  Not creating /var/log/minecraft."
else
	echo "Creating directory /var/log/minecraft"
	mkdir /var/log/minecraft
	chmod -R 755 /var/log/minecraft
	chown -R minecraft:minecraft /var/log/minecraft
fi
if [ -d "/var/log/minecraft/servers" ]
then 
	echo "Directory /var/log/minecraft/servers exists.  Not creating /var/log/minecraft/servers."
else
	echo "Creating directory /var/log/minecraft/servers"
	mkdir /var/log/minecraft/servers
	chmod -R 755 /var/log/minecraft/servers
	chown -R minecraft:minecraft /var/log/minecraft/servers
fi
if [ -e "/etc/minecraft/log4cpp.properties" ]
then 
	echo "File /etc/minecraft/log4cpp.properties exists.  Not creating /etc/minecraft/log4cpp.properties."
else
	echo "Creating file /etc/minecraft/log4cpp.properties"
	cp log4cpp.properties /etc/minecraft/log4cpp.properties
	chmod 755 /etc/minecraft/log4cpp.properties
	chown minecraft:minecraft /etc/minecraft/log4cpp.properties
fi
if [ -e "/etc/minecraft/config.json" ]
then 
	echo "File /etc/minecraft/config.json exists.  Not creating /etc/minecraft/config.json."
else
	echo "Creating file /etc/minecraft/config.json"
	cp config.json /etc/minecraft/config.json
	chmod 755 /etc/minecraft/config.json
	chown minecraft:minecraft /etc/minecraft/config.json
fi
if [ -d "/etc/bash_completion.d" ]
then
	echo "Installing bash autocompletion"
	cp bash_completion.sh /etc/bash_completion.d/minecraft
	chmod 644 /etc/bash_completion.d/minecraft
	chown root:root /etc/bash_completion.d/minecraft
else 
	echo "Directory /etc/bash_completion.d not found.  Not installing bash autocompletion."
fi
exit 0;
