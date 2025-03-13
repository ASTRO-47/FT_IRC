
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
    std::string                     _message;
    Client                          *_reciever;
    bool                            _disconnected;
    std::vector<std::string>        _command_buffer;
    std::string                     _ip;
    bool                            first;
    std::vector<std::string>        replys;
    std::string                     _second_buffer;
public:
    Client();
    size_t                         get_replys_size();
    bool                           check_all() const;
    void                           disconnected();
    bool                           check_pass() const;
    size_t                         get_buffer_size() const;
    std::string&                   get_cmd(int);
    std::vector<std::string>&      get_cmd_buffer();
    void                           parse_command();
    void                           add_server_to_poll(int);
    int                            get_socket_fd() const;
    void                           connect(int);
    pollfd                         &get_socket_struct();
    socklen_t                      &get_socket_addr_length();
    void                           append_buffer(std::string);
    const std::string              &get_buffer() const;
    void                           correct_pass();
    void                           wrong_pass();
    void                           set_nick_name(std::string);
    std::string                    get_nick_name() const; 
    void                           reset();
    void                           showed_messgae();
    bool                           check_message() const;
    void                           set_user_infos();
    bool                           cmd_end();
    void                           trim_message();
    bool                           check_nick() const;
    std::string                    get_message() const;
    void                           set_reciever(Client *);
    Client*                        get_reciever() const;
    bool                           check_connection();
    std::string                    get_hostname() const;
    std::string                    get_ip() const;
    void                           add_message(std::string);
    bool                           check_first_nick();
    void                           send_buffer();
    void                           set_buffer(std::string);             
    void                           clear_buffer();                 void            set_ip(std::string );
    ~Client();
} ;
