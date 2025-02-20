#include "server.hpp"


void Server::parse_nick(int i)
{
    clients[i]->set_nick_name();
}

void Server::parse_user(int i)
{
    if (clients[i]->check_all())
        send(clients[i]->get_socket_fd(), "You are already connected and cannot handshake again\n", 53, 0);
    else if (clients[i]->get_buffer_size() != 5)
        send(clients[i]->get_socket_fd(), "inavlid number of arguments\n", 29, 0);
    else
        clients[i]->set_user_infos();
}

void Server::try_to_auth(int i)
{
    if (clients[i]->get_buffer_size() == 2)
    {
        if (clients[i]->check_pass())
            send(clients[i]->get_socket_fd(), "you already validate the password\n", 34, 0);
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
        clients[i]->reset();
        return ;
    }
    if (clients[i]->get_cmd(0) == "PASS")
        try_to_auth(i);
    if (clients[i]->get_cmd(0) == "NICK" && clients[i]->get_buffer_size() > 1)
        parse_nick(i);
    if (clients[i]->get_cmd(0) == "USER")
        parse_user(i);
    if (clients[i]->check_all())
        registration_msge(i);
    clients[i]->reset();
}
