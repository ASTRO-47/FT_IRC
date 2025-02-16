#pragma once

#include "server.hpp"

class Client
{
private:
    std::string name;
public:
    Client(std::string);
    Client(const Client &);
    Client& operator=(const Client &);
    Client();
    ~Client();
} ;
