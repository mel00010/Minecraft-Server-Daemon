<?php

namespace Mel00010\MinecraftServerManager\WebSocket;

use Ratchet\ConnectionInterface;

interface ClientRepositoryInterface
{
    public function getClientByName($name);

    public function getClientByConnection(ConnectionInterface $conn);

    public function addClient(ConnectionInterface $conn);

    public function removeClient(ConnectionInterface $conn);

    public function getClients();
}
