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
#include <sstream>
#include <algorithm>
#include "client.hpp"
#include <string>

class Client;

class Server
{
    private:
        std::vector<Client *>       clients;
        std::string                 message;
        std::string                 _draw;
        std::vector<struct pollfd>  _poll_fds;
        std::string                 password;
        int                         port;
        int                         server_socket;
        sockaddr_in                 sock_addr; // use sockaddr_in6 when want to work with ipv6
        void                        handle_new_client();
        void                        handle_event_fd(int);
        void                        try_to_auth(int);
        void                        handle_cmd(int);
        void                        parse_nick(int);
        void                        parse_user(int);
        void                        registration_msge(int);
        std::string                 server_prefix;
        void                        taken_nick_name(int);
        bool                        taken_nick_name_1(std::string) const;
        // void                        disconnect_client(Client *);
        void                         handle_cmd_1(int);
        bool                         check_user(int);
        void                         handle_prv_msge(int);
        void                         send_reply(int, std::string);
        void                         send_private_message(Client *, std::string);
    public:
        void server_setup(std::string, std::string);
        void multiplexing_func();

        Server();
        ~Server();
};
