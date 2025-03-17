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
#include <string>
#include <exception>
#include <set>
#include <ctime>
#include <cmath>

#define CRLF (std::string)"\r\n"

class Bot
{
private:
    unsigned int                                         _port;
    std::string                                          _ip;
    std::string                                          _nick;
    std::string                                          _pass;
    std::string                                          _user;
    int                                                  _socket;
    sockaddr_in                                          _addr;
    std::vector<std::string>                             _jokes;
    std::map<std::string, std::string>                  _capitals;
    void                                                 random_joke(std::string &);
    void                                                 capital_handler(std::string&, std::string&);
public:
    Bot();
    ~Bot();
    void                                                parse(char **av);
    void                                                _connect();
    int                                                 which_service(std::string &);
    void                                                check_port(std::string);
};