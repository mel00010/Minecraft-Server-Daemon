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
groupadd minecraft || echo "Group minecraft already exists"
useradd -gminecraft -m minecraft || echo "User minecraft already exists"
if [ -d "$sysconfdir/$CONFIG_DIR_NAME" ]
then 
	echo "Directory /usr/local/etc/$CONFIG_DIR_NAME exists.  Not creating /usr/local/etc/$CONFIG_DIR_NAME."
else
	echo "Creating directory /usr/local/etc/$CONFIG_DIR_NAME"
	mkdir /usr/local/etc/$CONFIG_DIR_NAME
	chmod -R 755 /usr/local/etc/$CONFIG_DIR_NAME
	chown -R minecraft:minecraft /usr/local/etc/$CONFIG_DIR_NAME
fi
if [ -d "$LOG_DIR" ]
then
	echo "Directory $LOG_DIR exists.  Not creating $LOG_DIR."
else
	echo "Creating directory $LOG_DIR"
	mkdir $LOG_DIR
	chmod -R 755 $LOG_DIR
	chown -R minecraft:minecraft $LOG_DIR
fi
if [ -d "/var/log/minecraft/servers" ]
then 
	echo "Directory $LOG_DIR/servers exists.  Not creating $LOG_DIR/servers."
else
	echo "Creating directory $LOG_DIR/servers"
	mkdir $LOG_DIR/servers
	chmod -R 755 $LOG_DIR/servers
	chown -R minecraft:minecraft $LOG_DIR/servers
fi
if [ -e "$CONFIG_FILE_NAME" ]
then 
	echo "File $CONFIG_FILE_NAME exists.  Not creating $CONFIG_FILE_NAME."
else
	echo "Creating file $CONFIG_FILE_NAME"
	cp config/config.json $CONFIG_FILE_NAME
	chmod 755 $CONFIG_FILE_NAME
	chown minecraft:minecraft $CONFIG_FILE_NAME
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
exit 0