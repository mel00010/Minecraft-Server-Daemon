<?php

namespace Mel00010\MinecraftServerManager;

use Mel00010\MinecraftServerManager\WebSocket\ClientRepository;
use Ratchet\MessageComponentInterface;
use Ratchet\ConnectionInterface;

class Server implements MessageComponentInterface {
	public $repository;
	protected $loop;
	protected $socket;
	/**
	 * Chat Constructor
	 */
	public function __construct($loop) {
		syslog ( LOG_DEBUG, "In Server constructor" );
		$this->repository = new ClientRepository ();
		$this->loop = $loop;
		syslog ( LOG_DEBUG, "Opening socket" );
		$this->socket = new Connection\Socket ();
		$this->socket->open ( array (
				$this,
				'onSocketData' 
		), $loop );
	}
	
	/**
	 * Called when a connection is opened
	 *
	 * @param ConnectionInterface $conn        	
	 * @return void
	 */
	public function onOpen(ConnectionInterface $conn) {
		syslog ( LOG_DEBUG, "In onOpen" );
		$client = $this->repository->addClient ( $conn );
		syslog ( LOG_INFO, "Client " . $client->getName () . " connected" );
		// $this->readStatus();
	}
	
	/**
	 * Called when a message is sent through the socket
	 *
	 * @param ConnectionInterface $conn        	
	 * @param string $msg        	
	 * @return void
	 */
	public function onMessage(ConnectionInterface $conn, $data) {
		// json_decode ( $data, true );
		syslog ( LOG_DEBUG, "In onMessage" );
		$command = json_decode ( $data, true );
		$message = new Connection\Message ();
		switch ($command ['command']) {
			case "backupServer" :
				$message->command = "backupServer";
				$message->server = $command ['server'];
				break;
			case "backupAll" :
				$message->command = "backupAll";
				break;
			case "listServers" :
				$message->command = "listServers";
				break;
			case "listOnlinePlayers" :
				if ($command ['player'] != "") {
					$message->command = "listOnlinePlayersFiltered";
					$message->server = $command ['server'];
					$message->player = $command ['player'];
				} else {
					$message->command = "listOnlinePlayers";
					$message->server = $command ['server'];
				}
				break;
			case "registerOutputListener" :
				$message->command = "registerOutputListener";
				$message->server = $command ['server'];
				break;
			case "unRegisterOutputListener" :
				$message->command = "unRegisterOutputListener";
				$message->server = $command ['server'];
				break;
			case "restartServer" :
				$message->command = "restartServer";
				$message->server = $command ['server'];
				break;
			case "restartAll" :
				$message->command = "restartAll";
				break;
			case "sendCommand" :
				$message->command = "sendCommand";
				$message->server = $command ['server'];
				$message->serverCommand = $command ['serverCommand'];
				break;
			case "startServer" :
				$message->command = "startServer";
				$message->server = $command ['server'];
				break;
			case "serverStatus" :
				$message->command = "serverStatus";
				$message->server = $command ['server'];
				break;
			case "stopServer" :
				$message->command = "stopServer";
				$message->server = $command ['server'];
				break;
			case "stopAll" :
				$message->command = "stopAll";
				break;
			case "stopDaemon" :
				$message->command = "stopDaemon";
				break;
			case "updateServer" :
				$message->command = "updateServer";
				$message->server = $command ['server'];
				$message->version = $command ['version'];
				break;
			case "updateAll" :
				$message->command = "updateAll";
				$message->version = $command ['version'];
				break;
			case "opPlayer" :
				$message->command = "opPlayer";
				$message->server = $command ['server'];
				break;
			case "deopPlayer" :
				$message->command = "deopPlayer";
				$message->server = $command ['server'];
				break;
			case "banPlayer" :
				if ($command ['reason'] != "") {
					$message->command = "banPlayerReason";
					$message->server = $command ['server'];
					$message->reason = $command ['reason'];
				} else {
					$message->command = "banPlayer";
					$message->server = $command ['server'];
				}
				break;
			case "pardonPlayer" :
				$message->command = "pardonPlayer";
				$message->server = $command ['server'];
				break;
			case "kickPlayer" :
				if ($command ['reason'] != "") {
					$message->command = "kickPlayerReason";
					$message->server = $command ['server'];
					$message->player = $command ['player'];
					$message->reason = $command ['reason'];
				} else {
					$message->command = "kickPlayer";
					$message->server = $command ['server'];
					$message->player = $command ['player'];
				}
				break;
			case "runScript" :
				$message->command = "runScript";
				$message->script = $command ['script'];
				break;
			default :
				break;
		}
		$this->socket->sendMessage ( $message );
	}
	// exit();
	public function onClose(ConnectionInterface $conn) {
		syslog ( LOG_DEBUG, "In onClose" );
		// The connection is closed, remove it, as we can no longer send it messages
		$client = $this->repository->getClientByConnection ( $conn );
		syslog ( LOG_INFO, "Client " . $client->getName () . " disconnected" );
		$this->repository->removeClient ( $conn );
	}
	/**
	 * Called when an error occurs on a connection
	 *
	 * @param ConnectionInterface $conn        	
	 * @param Exception $e        	
	 * @return void
	 */
	public function onError(ConnectionInterface $conn, \Exception $e) {
		syslog ( LOG_DEBUG, "In onError" );
		syslog ( LOG_INFO, "The following error occured: " . $e->getMessage () );
		
		$client = $this->repository->getClientByConnection ( $conn );
		
		// We want to fully close the connection
		if ($client !== null) {
			$client->getConnection ()->close ();
			$this->repository->removeClient ( $conn );
		}
	}
	public function onSocketData($data) {
		syslog ( LOG_DEBUG, "In onSocketData" );
		syslog ( LOG_DEBUG, $data );
		$dataArray = array_filter ( explode ( "\n", $data ) );
		foreach ( $dataArray as $line ) {
			
		}
		foreach ( $dataArray as $line ) {
			$cutLine = substr ( $line, 8 );
			foreach ( $this->repository->getClients () as $client ) {
				$client->send ( $cutLine );
			}
// 			var_dump($cutLine);
		}
	}
}
