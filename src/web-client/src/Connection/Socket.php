<?php

namespace Mel00010\MinecraftServerManager\Connection;

class Socket {
	protected $socket;
	protected $stream;
	protected $loop;
	public function open(callable $callbackFunction, $loop) {
		$this->loop = $loop;
		$this->socket = @stream_socket_client ( 'unix://' . '/home/mel/workspace/web-client/socket.socket', $errno, $errstr, 1.0 );
		
		// $this->socket = @stream_socket_client('unix://' . '/etc/minecraft/control.socket', $errno, $errstr, 1.0);
		$this->stream = new \React\Stream\Stream ( $this->socket, $this->loop );
		// syslog ( LOG_DEBUG, "In Socket open anonymous function" );
		syslog ( LOG_DEBUG, "Setting callback function" );
		$this->stream->on ( 'data', $callbackFunction );
	}
	public function close() {
		$this->stream->end ();
	}
	public function getStream() {
		return $this->stream;
	}
	public function getSocket() {
		return $this->socket;
	}
	public function sendMessage(Message $data) {
		$encodedMessage = json_encode ( $data ) . PHP_EOL ;
		$encodedMessageLength = substr ( str_pad ( strlen ( $encodedMessage ), 8, "0", STR_PAD_LEFT ), 0, 8 );
		syslog ( LOG_DEBUG, $encodedMessageLength . $encodedMessage);
		$this->stream->write ( $encodedMessageLength . $encodedMessage);
	}
}