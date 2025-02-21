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
    if (clients[i]->check_message())
    {
        std::string msge = server_prefix + clients[i]->get_nick_name() + ": You already connected and cannot handshake again";
        send(clients[i]->get_socket_fd(), msge.c_str(), msge.length(), 0);
    }
    if (clients[i]->get_buffer_size() == 2)
    {
        if (clients[i]->get_cmd(1) == password)
            clients[i]->correct_pass();
        else
        {
            std::string msge = server_prefix + clients[i]->get_cmd(1) + " :password incorrect\n";
            send(clients[i]->get_socket_fd(), msge.c_str(), msge.length(), 0);
            clients[i]->wrong_pass();
        }
    }
    else
    {
        send(clients[i]->get_socket_fd(), "wrong password\n", 15, 0);
    }
}
 
void Server::handle_cmd(int i)
{
    clients[i]->parse_command();
    if (!clients[i]->check_pass() && clients[i]->get_cmd(0) != "PASS")
    {
        // remember to add return code
        std::string msge = server_prefix + clients[i]->get_nick_name() + " :You have not registered\n";
        send(clients[i]->get_socket_fd(), msge.c_str(), msge.length(), 0);
        clients[i]->reset();
        return ;
    }
    if (clients[i]->get_cmd(0) == "PASS")
        try_to_auth(i);
    if (clients[i]->get_cmd(0) == "NICK" && clients[i]->get_buffer_size() > 1)
        parse_nick(i);
    if (clients[i]->get_cmd(0) == "USER")
        parse_user(i);
    if (clients[i]->check_all() && !clients[i]->check_message())
        registration_msge(i);
    clients[i]->reset();
}
