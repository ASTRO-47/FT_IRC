#include "server.hpp"

void Server::taken_nick_name(Client *C)
{
    std::string _n = C->get_nick_name();
    
    for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); it++)
    {
        if ((*it)->get_nick_name() == _n && !(*it)->check_all())  
        {
            std::string msge = "ERROR :Closing Link: " + _n + " by :ft_irc (Overridden by other sign on)\n";
            send_reply((*it)->get_socket_fd(), msge);
            close((*it)->get_socket_fd());
            // _poll_fds.erase(_poll_fds.begin() + i);
            delete *(it);
            it = clients.erase(it);
            // i--;
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

void Server::parse_nick(Client *C)
{
    std::string msge;
    if (C->get_buffer_size() == 1) // add the nick name is in use
    {
        msge = server_prefix + "431 :No nickname given\n";
        send_reply(C->get_socket_fd(), msge);
    }
    else
    {
        if (taken_nick_name_1(C->get_cmd(1)))
        {
            if (C->check_nick())
            {
                msge = C->get_nick_name() + "!@ NICK :" + C->get_cmd(1) + '\n';
                send_reply(C->get_socket_fd(), msge);
                // send info to all the joined channels that the nik is changed
            }
            C->set_nick_name();
        }
        else
        {
            msge = server_prefix + "433 " +  C->get_cmd(1) +  " :Nickname is already in use\n";
            send_reply(C->get_socket_fd(), msge);
        }
    }
}

void Server::parse_user(Client *C)
{
    std::string  msge;
    if (C->check_all())
    {
        msge = server_prefix + "462 " + C->get_nick_name() + " :You may not reregister\n";
        send_reply(C->get_socket_fd(), msge);
    }
    else if (C->get_buffer_size() < 5)
    {
        
        msge = server_prefix + "461 " +  "USER :Not enough parameters\n";
        send_reply(C->get_socket_fd(), msge);
    }
    else
        C->set_user_infos();
}

void Server::try_to_auth(Client *C)
{
    std::string _n = C->get_nick_name();
    if (C->check_message())
    {
        std::string msge = server_prefix + "462 " + C->get_nick_name() + " :You may not reregister\n";
        send_reply(C->get_socket_fd(), msge);
    }
    else if (C->get_buffer_size() == 1)
    {
        std::string msge = server_prefix + "461 PASS: Not enough parameters\n";   
        send_reply(C->get_socket_fd(), msge);
    }
    else if (C->get_buffer_size() > 1)
    {
        if (C->get_cmd(1) == password)
            C->correct_pass();
        else
        {
            std::string msge = server_prefix + "464 " +  C->get_cmd(1) + " :Password incorrect\n";
            send_reply(C->get_socket_fd(), msge);
            C->wrong_pass();
        }
    }
}

void Server::handle_cmd(Client *C)
{
    C->parse_command();

    if (!C->check_pass() && (C->get_cmd(0) != "pass" && C->get_cmd(0) != "PASS"))
    {
        std::string msge = server_prefix + "451 :You have not registered\n";
        send_reply(C->get_socket_fd(), msge);
        C->reset();
        return ;
    }
    if (C->get_cmd(0) == "pass" || C->get_cmd(0) == "PASS")
        try_to_auth(C);
    if (C->get_cmd(0) == "nick" || C->get_cmd(0) == "NICK")
        parse_nick(C);
    if (C->get_cmd(0) == "user" || C->get_cmd(0) == "USER")
        parse_user(C);
    if (C->check_all() && !C->check_message())
        registration_msge(C);
    C->reset();
}
