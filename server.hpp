#pragma once

#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <map>
#include <vector>
#include <poll.h>
#include "client.hpp"

class Client;

class Server
{
    private:
        std::vector<Client *> clients;
        std::vector<struct pollfd>  _poll_fds;
        std::string password;
        int port;
        int server_socket;
        sockaddr_in sock_addr;
        void handle_new_client();
        void handle_event_fd(int);
    public:
        void server_setup(std::string, std::string);
        void multiplexing_func();
        
        Server();
        ~Server();
};
