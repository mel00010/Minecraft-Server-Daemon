<?php
namespace Mel00010\MinecraftServerManager\WebSocket;
use Ratchet\Server\IoServer as IoServerOld;
class IoServer extends IoServerOld {
    public static function factory(MessageComponentInterface $component, $port = 80, $address = '0.0.0.0') {
        $loop   = LoopFactory::create();
        $socket = new Reactor($loop);
        $socket->listen($port, $address);
        $sock = socket_import_stream($socket->master);
        socket_set_option($sock, SOL_TCP, TCP_NODELAY, true);
        return new static($component, $socket, $loop);
    }
}
?>
