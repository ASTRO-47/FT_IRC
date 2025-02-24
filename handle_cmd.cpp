#include "server.hpp"

void Server::taken_nick_name(Client *_C)
{
    std::string _n = _C->get_nick_name();

    for (std::vector<Client *>::const_iterator it = clients.begin(); it != clients.end(); it++)
    {
        if ((*it)->get_nick_name() == _n && !(*it)->check_all())  // Ensure the check is active
        {
            std::string msge = "ERROR :Closing Link: " + _n + " by :ft_irc (Overridden by other sign on)\n";
            send((*it)->get_socket_fd(), msge.c_str(), msge.length(), 0);
            close ((*it)->get_socket_fd());
            // _poll_fds.erase(_C->get_socket_struct());
            // delete clients[i];
            // clients.erase(clients.begin() + i);
        }
    }
}

bool Server::taken_nick_name_1(std::string _n) const
{
    for (std::vector<Client *>::const_iterator it = clients.begin(); it != clients.end(); it++)
    {
        if ((*it)->get_nick_name() == _n && (*it)->check_all())  // Ensure the check is active
            return false;
    }
    return true;
}

void Server::parse_nick(int i)
{
    std::string msge;
    if (clients[i]->get_buffer_size() == 1) // add the nick name is in use
    {
        msge = server_prefix + "431 :No nickname given\n";
        send(clients[i]->get_socket_fd(), msge.c_str(), msge.length(), 0);
    }
    else
    {
        if (taken_nick_name_1(clients[i]->get_cmd(1)))
            clients[i]->set_nick_name();
        else
        {
            msge = server_prefix + "433 " +  clients[i]->get_cmd(1) +  " :Nickname is already in use\n";
            send(clients[i]->get_socket_fd(), msge.c_str(), msge.length(), 0);
        }
    }
}

void Server::parse_user(int i)
{
    std::string  msge; 
    if (clients[i]->check_all())
    {
        msge = server_prefix + "462 " + clients[i]->get_nick_name() + " :You may not reregister\n";
        send(clients[i]->get_socket_fd(), msge.c_str(), msge.length(), 0);
    }
    else if (clients[i]->get_buffer_size() < 5)
    {
        
        msge = server_prefix + "461 " +  "USER :Not enough parameters\n";
        send(clients[i]->get_socket_fd(), msge.c_str(), msge.length(), 0);
    }
    else
        clients[i]->set_user_infos();
}

void Server::try_to_auth(int i)
{
    std::string _n = clients[i]->get_nick_name();
    if (clients[i]->check_message())
    {
        std::string msge = server_prefix + "462 " + clients[i]->get_nick_name() + " :You may not reregister\n";
        send(clients[i]->get_socket_fd(), msge.c_str(), msge.length(), 0);
    }
    else if (clients[i]->get_buffer_size() == 1)
    {
        std::string msge = server_prefix + "461 PASS: Not enough parameters\n";   
        send(clients[i]->get_socket_fd(), msge.c_str(), msge.length(), 0);
    }
    else if (clients[i]->get_buffer_size() > 1)
    {
        if (clients[i]->get_cmd(1) == password)
            clients[i]->correct_pass();
        else
        {
            std::string msge = server_prefix + "464 " +  clients[i]->get_cmd(1) + " :Password incorrect\n";
            send(clients[i]->get_socket_fd(), msge.c_str(), msge.length(), 0);
            clients[i]->wrong_pass();
        }
    }
}

void Server::handle_cmd(int i)
{
    clients[i]->parse_command();

    if (!clients[i]->check_pass() && (clients[i]->get_cmd(0) != "pass" && clients[i]->get_cmd(0) != "PASS"))
    {
        std::string msge = server_prefix + "451 :You have not registered\n";
        send(clients[i]->get_socket_fd(), msge.c_str(), msge.length(), 0);
        clients[i]->reset();
        return ;
    }
    if (clients[i]->get_cmd(0) == "pass" || clients[i]->get_cmd(0) == "PASS")
        try_to_auth(i);
    if (clients[i]->get_cmd(0) == "nick" || clients[i]->get_cmd(0) == "NICK")
        parse_nick(i);
    if (clients[i]->get_cmd(0) == "user" || clients[i]->get_cmd(0) == "USER")
        parse_user(i);
    if (clients[i]->check_all() && !clients[i]->check_message())
        registration_msge(i);
    clients[i]->reset();
}
