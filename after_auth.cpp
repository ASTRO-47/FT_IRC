#include "server.hpp"

void    Server::handle_cmd_1(int i)
{
    // here marin should handle his things after client auth

    std::cout << clients[i]->get_buffer() ; // printing the cmd for debuginhg reason
    clients[i]->reset();
}