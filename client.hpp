#pragma once

#include "server.hpp"

class Client
{
private:
    bool                            _msg;
    std::string                     host_name;
    std::string                     user_name;
    std::string                     server_name;
    std::string                     nick;
    std::string                     real_name;
    bool                            _nick;
    bool                            pass;
    bool                            user;
    sockaddr_in                     socket_addr;
    socklen_t                       addr_len;
    pollfd                          __poll;
    int                             client_socket;
    std::string                     _buffer;
    std::vector<std::string>        _command_buffer;
public:
    Client();
    bool            check_all() const;
    bool            check_pass() const;
    int             get_buffer_size() const;
    std::string&    get_cmd(int);
    void            parse_command();
    void            add_server_to_poll(int);
    int             get_socket_fd() const;
    void            connect(int);
    pollfd          get_socket_struct();
    socklen_t       &get_socket_addr_length();
    void            append_buffer(char *);
    std::string     get_buffer() const;
    void            correct_pass();
    void            wrong_pass();
    void            set_nick_name();
    std::string     get_nick_name() const; 
    void            reset();
    void            showed_messgae();
    bool            check_message() const;
    void            set_user_infos();
    bool            cmd_end() const;
    ~Client();
} ;
