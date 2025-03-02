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
        void                        handle_event_fd(Client *);
        void                        try_to_auth(Client *);
        void                        handle_cmd(Client *);
        void                        parse_nick(Client *);
        void                        parse_user(Client *);
        void                        registration_msge(Client *);
        std::string                 server_prefix;
        void                        taken_nick_name(Client *);
        bool                        taken_nick_name_1(std::string) const;
        void                         handle_cmd_1(Client *);
        bool                         check_user(Client *);
        void                         handle_prv_msge(Client *);
        void                         send_reply(int, std::string);
        void                         send_private_message(Client *, std::string);
        void                         change_nick_name(Client *);
    public:
        void server_setup(std::string, std::string);
        void multiplexing_func();

        Server();
        ~Server();
};
