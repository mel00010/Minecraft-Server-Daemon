#!/usr/local/bin/php
<?php

require __DIR__ . '/vendor/autoload.php';
openlog("MinecraftServerManager",LOG_NDELAY|LOG_PID, LOG_LOCAL0);
$loop = React\EventLoop\Factory::create();

$webSock = new React\Socket\Server($loop);
$webSock->listen(8080, '0.0.0.0'); // Binding to 0.0.0.0 means remotes can connect
$webServer = new Ratchet\Server\IoServer(
	new Ratchet\Http\HttpServer(
		new Ratchet\WebSocket\WsServer(
			new Mel00010\MinecraftServerManager\Server($loop)
		)
	),
	$webSock
);
syslog(LOG_DEBUG, "Constructed server");
$loop->run();
