<?php

namespace Mel00010\MinecraftServerManager\WebSocket;

use SplObjectStorage;
use Mel00010\MinecraftServerManager\WebSocket\ClientConnection;
use Ratchet\ConnectionInterface;

class ClientRepository implements ClientRepositoryInterface
{
    /**
     * All the connected clients
     *
     * @var SplObjectStorage
     */
    private $clients;

    /**
     * ChatRepository Constructor
     */
    public function __construct()
    {
        $this->clients = new SplObjectStorage;
    }

    /**
     * Get a client by their name
     *
     * @param string $name
     * @return ChatConnectionInterface
     */
    public function getClientByName($name)
    {
        foreach ($this->clients as $client)
        {
            if ($client->getName() === $name)
                return $client;
        }

        return null;
    }

    /**
     * Get a client by their connection
     *
     * @param ConnectionInterface $conn
     * @return ChatConnectionInterface
     */
    public function getClientByConnection(ConnectionInterface $conn)
    {
        foreach ($this->clients as $client)
        {
            if ($client->getConnection() === $conn)
            return $client;
        }

        return null;
    }

    /**
     * Add a client to the list
     *
     * @param ConnectionInterface $conn
     * @return void
     */
    public function addClient(ConnectionInterface $conn)
    {
		$client =new ClientConnection($conn, $this);
        $this->clients->attach($client);
        return $client;
    }

    /**
     * Remove a client from the list
     *
     * @param ConnectionInterface $conn
     * @return void
     */
    public function removeClient(ConnectionInterface $conn)
    {
        $client = $this->getClientByConnection($conn);

        if ($client !== null)
            $this->clients->detach($client);
    }

    /**
     * Get all the connected clients
     *
     * @return SplObjectStorage
     */
    public function getClients()
    {
        return $this->clients;
    }
}
