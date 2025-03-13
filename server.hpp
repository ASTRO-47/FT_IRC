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
#include <sstream>
#include "utils.hpp"
#include "replies.hpp"
#include "Channel.hpp"
#include <exception>


#define CRLF (std::string)"\r\n"
#define MAX_NICK_LEN 9 // think about this later

class Client;
class Channel;

class Server
{
    private:
        static Server                *ins;
        std::string                  auth_guide;
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
        void                         check_port(std::string);
        void                         change_nick_name(int);
        bool                         kick_user(Client *, std::string&, std::string&);
        std::map<std::string ,Channel>channelMap;
        std::vector<std::pair<std::string, std::string> >channelAndPass;
        std::set<char> modes;
        Client * find_client(std::string &);
        void extract_channels(const std::string &, const std::string &);
        bool channel_exists(const std::string&);
        void create_channel(const std::string&, Client *);
        std::string parse_join_input(const std::string &, size_t &);
        std::string parse_passwords(const std::string &, size_t &);
        void check_operations(const std::string &, Client&, Channel &);
        void process_operation(char, const char &, Client &,std::string &, Channel &);
        Client* find_user(const std::string &, Channel &);
        void append_user_to_channel(Channel& ,Client *);
        bool requiresArg(char, char );
        void invite_user(const std::string &, Client *, const std::string &);
        void join_handler(Client &);
        void mode_handler(Client &);
        void topic_handler(Client &, size_t);
        void kick_handler(Client &, size_t);
        bool                         check_nick_name(int);
        bool                         check_command(int);
        void                         handle_quit_cmd(int);
        void                         broadcast_msge(std::string);
        void                         check_pass(std::string);
        static void                  handler(int);
        void                         clean_server();
        void                         removeChannel();
        void removeUserFromChannels(Client &);
    public:
        void server_setup(std::string, std::string);
        void multiplexing_func();
        void broadcastMsg(std::string);
        static void send_reply(int, std::string);

        Server();
        ~Server();
};
