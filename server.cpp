#include "server.hpp"

Server::Server(int _port) : port(_port)
{
    if (port < -1)
    {
        throw std::runtime_error("invalid port, please chose some thing  >1023");
    }
}

Server::~Server(){}