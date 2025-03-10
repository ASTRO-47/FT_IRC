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
#include <exception>
#include <set>

#define MAX_NICK_LEN 9 // think about this later

class Client;
class Channel;

class Server
{
    private:
        int                          bot_fd;
        bool                         bot_status;
        std::vector<Client *>        clients;
        std::string                  message;
        std::string                  _draw;
        std::vector<struct pollfd>   _poll_fds;
        std::string                  password;
        int                          port;
        int                          server_socket;
        sockaddr_in                  sock_addr; // use sockaddr_in6 when want to work with ipv6
        void                         handle_new_client();
        void                         handle_event_fd(int);
        void                         try_to_auth(int);
        void                         handle_cmd(int);
        void                         parse_nick(int);
        void                         parse_user(int);
        void                         registration_msge(int);
        std::string                  server_prefix;
        void                         taken_nick_name(int);
        bool                         taken_nick_name_1(std::string) const;
        void                         handle_cmd_1(int);
        bool                         check_user(int);
        void                         handle_prv_msge(int);
        void                         send_reply(int, std::string);
        void                         check_port(std::string);
        void                         change_nick_name(int);

        std::map<std::string ,Channel*>channelMap;
        std::map<std::string, std::string>channelAndPass;
        void extract_channels(const std::string &, int, const std::string &);
        bool channel_exists(const std::string&);
        void create_channel(const std::string&, Client *);
        std::string parse_join_input(const std::string &, size_t &);
        std::string parse_passwords(const std::string &, size_t &);
        // bool valid_channel(const std::string &);
        void check_operations(const std::string &, int, Channel *);
        void process_operation(char, const char &, int, Channel *);
        void find_user(const std::string &, int, Channel *);
        void append_user_to_channel(Channel * ,Client *);
        std::set<char> modes;
        bool                         check_nick_name(int);
        bool                         check_command(int);
        void                         handle_quit_cmd(int);
        void                         broadcast_msge(std::string);
        void                         handle_bot_cmd(int);
        void                         check_pass(std::string);
    public:
        void server_setup(std::string, std::string);
        void multiplexing_func();

        Server();
        ~Server();
};
