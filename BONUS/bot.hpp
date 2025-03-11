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

#define CRLF (std::string)"\r\n"

class Bot
{
private:
    unsigned int                         _port;
    std::string                 _ip;
    std::string                 _nick;
    std::string                 _pass;
    std::string                 _user;
    int                         _socket;
    sockaddr_in                 _addr;
    std::vector<std::string>    _jokes;
    std::string                 random_joke();
public:
    Bot();
    void                parse(char **av);
    void                _connect();
} ;