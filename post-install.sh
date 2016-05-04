#!/bin/bash
groupadd minecraft
useradd -gminecraft -m minecraft
#~ usermod –gminecraft minecraft
mkdir /etc/minecraft
mkdir /var/log/minecraft
mkfifo /etc/minecraft/control.pipe
mkfifo /etc/minecraft/output.pipe
cp log4cpp.properties /etc/minecraft/log4cpp.properties
cp config.json /etc/minecraft/config.json
chmod -R 644 /etc/minecraft
chmod -R 644 /var/log/minecraft
chmod 666 /etc/minecraft/control.pipe
chmod 644 /etc/minecraft/output.pipe
chown -R minecraft:minecraft /etc/minecraft
chown -R minecraft:minecraft /var/log/minecraft
exit 0;
