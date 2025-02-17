#include "client.hpp"

Client::Client(std::string _name) : auth(false)
{
    //check the username policy and duplicate
    name = _name;
}

