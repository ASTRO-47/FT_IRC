#pragma once

#include "server.hpp"

class Client
{
private:
    std::string     name;
    bool            pass;
    bool            auth;
    sockaddr_in     socket_addr;
    socklen_t       addr_len;
    pollfd          __poll;
    int             client_socket;
    std::string     _buffer;
public:
    Client();
    bool        check_pass() const;
    bool        check_auth() const;
    void        add_server_to_poll(int);
    int         get_socket_fd() const;
    void        connect(int);
    sockaddr_in &get_socket_addr();
    pollfd      get_socket_struct();
    socklen_t   &get_socket_addr_length();
    void        append_buffer(char *);
    std::string get_buffer() const;
    void        reset();
    ~Client();
} ;

// hello world how the hell are you doing