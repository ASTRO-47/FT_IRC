#include "server.hpp"


void Server::parse_nick()
{
    
}

void Server::try_to_auth(int i)
{
    if (clients[i]->get_buffer_size() == 2)
    {
        if (clients[i]->check_pass())
            send(clients[i]->get_socket_fd(), "you already validate the password\n", 33, 0);
        else
        {
            if (clients[i]->get_cmd(1) == password)
                clients[i]->correct_pass();
            else
                send(clients[i]->get_socket_fd(), "wrong password\n", 15, 0);
        }    
    }
}

void Server::handle_cmd(int i)
{
    clients[i]->parse_command();
    if (!clients[i]->check_pass() && clients[i]->get_cmd(0) != "PASS")
    {
        send(clients[i]->get_socket_fd(), "you have to validate the password first\n", 40, 0);
        return ;
    }
    if (clients[i]->get_cmd(0) == "PASS")
        try_to_auth(i);
    if (clients[i]->get_cmd(0) == "NICK" && clients[i]->get_buffer_size() > 1)
        parse_nick();
    // clients[i]->reset();
    // if (!_C->check_auth())
    //     try_to_auth(_C);
    // send(_C->get_socket_fd(), "you are not registered\n", 24, 0);
    clients[i]->reset();
}
