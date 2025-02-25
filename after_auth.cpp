#include "server.hpp"

void Server::handle_prv_msge(int i)
{
    clients[i]->trim_message();
    std::string msge = server_prefix + clients[i]->get_nick_name() +  " PRIVMSG :" + clients[i]->get_message() + '\n';
    send(clients[i]->get_reciever()->get_socket_fd(), msge.c_str(), msge.length(), 0);
}

bool    Server::check_user(int i)
{
    std::string _n = clients[i]->get_cmd(1);

    for (std::vector<Client *>::const_iterator it = clients.begin(); it != clients.end(); it++)
    {
        if ((*it)->get_nick_name() == _n && (*it)->check_all())
        {
            clients[i]->set_reciever((*it));
            return true;
        }
    }
    std::string msge =  "402 " + _n +  " :No such nick/channel\n";
    send(clients[i]->get_socket_fd(), msge.c_str(), msge.length(), 0);
    return false;
}

void    Server::handle_cmd_1(int i)
{
    clients[i]->parse_command();
    if (clients[i]->get_cmd(0) == "privmsg" || clients[i]->get_cmd(0) == "PRIVMSG")
    {
        // if () need to check for all invalid conditins
        if (!check_user(i))
        {
            clients[i]->reset();
            return ;
        }
        handle_prv_msge(i);
    }
    clients[i]->reset();
}