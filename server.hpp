#pragma once

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using std::cout, std::string, std::endl;

class Server
{
    private:
        sockaddr_in sock_addr;
        int port;
        int server_socket;
        int client_socket;
        // struct sockaddr_in server_address;
        // struct sockaddr_in client_address;
        // socklen_t client_address_len;
    public:
        void server_setup(std::string);
        void wait_connections();
        Server();
        ~Server();
};

//endianness