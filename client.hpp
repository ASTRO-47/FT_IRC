#pragma once

#include "server.hpp"

class Client
{
private:
    std::string     name;
    bool            auth;
public:
    Client(std::string);
    Client(const Client &);
    Client& operator=(const Client &);
    Client();
    ~Client();
} ;
