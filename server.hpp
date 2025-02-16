#pragma once

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <map>
// #include <poll>

class Server
{
    private:
        std::map<std::string , int> clients;
        int port;
        int server_socket;
        int client_socket;
        sockaddr_in sock_addr;
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