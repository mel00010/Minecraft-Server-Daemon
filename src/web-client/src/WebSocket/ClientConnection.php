<?php

namespace Mel00010\MinecraftServerManager\WebSocket;

use Mel00010\MinecraftServerManager\WebSocket\ClientRepositoryInterface;
use Ratchet\ConnectionInterface;

class ClientConnection implements ClientConnectionInterface
{
    /**
     * The ConnectionInterface instance
     *
     * @var ConnectionInterface
     */
    private $connection;

    /**
     * The username of this connection
     *
     * @var string
     */
    private $name;

    /**
     * The ChatRepositoryInterface instance
     *
     * @var ChatRepositoryInterface
     */
    private $repository;

    /**
     * ChatConnection Constructor
     *
     * @param ConnectionInterface     $conn
     * @param ChatRepositoryInterface $repository
     * @param string                  $name
     */
    public function __construct(ConnectionInterface $conn, ClientRepositoryInterface $repository, $name = "")
    {
        $this->connection = $conn;
        $this->name = uniqid();
        $this->repository = $repository;
    }

    /**
     * Sends a message through the socket
     *
     * @param string $sender
     * @param string $msg
     * @return void
     */
    public function sendMsg($msg)
    {
        $this->send($msg);
    }

    /**
     * Get the connection instance
     *
     * @return ConnectionInterface
     */
    public function getConnection()
    {
        return $this->connection;
    }

    /**
     * Set the name for this connection
     *
     * @param string $name
     * @return void
     */
    public function setName($name)
    {
        $this->name = $name;
    }

    /**
     * Get the username of the connection
     *
     * @return string
     */
    public function getName()
    {
        return $this->name;
    }

    /**
     * Send data through the socket
     *
     * @param  array  $data
     * @return void
     */
    public function send($data)
    {
//		$data['padding'] = 'padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding padding';
        $this->connection->send($data);
    }
}
