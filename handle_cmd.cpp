#include "server.hpp"

void Server::taken_nick_name(int i)
{
    std::string _n = clients[i]->get_nick_name();
    
    for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); it++)
    {
        if ((*it)->get_nick_name() == _n && !(*it)->check_all())
        {
            std::string msge = "ERROR :Closing Link: " + clients[i]->get_nick_name() + " by :ft_irc (Overridden by other sign on)\r\n";
            send_reply(clients[i]->get_socket_fd(), msge);
            close((*it)->get_socket_fd());
            (*it)->disconnected();
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

bool    Server::check_nick_name(int i) 
{
    std::string nick = clients[i]->get_cmd(1);

    if (!nick.length() || nick.length() > MAX_NICK_LEN)
        return false;
    if (!isalpha(nick[0]))
        return false;
    for (int i = 1; i < nick.length() ; i++) 
    {
        if (!isalnum(nick[i]) && nick[i] != '-' && nick[i] != '[' && nick[i] != ']' && nick[i] != '\\' && nick[i] != '_')
            return false;
    }
    return true;
}

void Server::parse_nick(int i)
{
    std::string msge;
    if (clients[i]->get_buffer_size() == 1) // add the nick name is in use
    {
        msge = server_prefix + "431 :No nickname given\r\n";
        send_reply(clients[i]->get_socket_fd(), msge);
    }
    else
    {
        if (!check_nick_name(i))
        {
            std::string msge = server_prefix +  "431 * " + clients[i]->get_cmd(1) +" :Erroneus nickname\r\n";
            send_reply(clients[i]->get_socket_fd(), msge);
        }
        else if (taken_nick_name_1(clients[i]->get_cmd(1)))
        {
            if (!clients[i]->check_all() && clients[i]->check_nick())
            {
                if (clients[i]->check_first_nick())
                {
                    msge = clients[i]->get_nick_name() + "!@ NICK :" + clients[i]->get_cmd(1) + '\n';
                    send_reply(clients[i]->get_socket_fd(), msge);
                }
                // send info to all the joined channels that the nik is changed
            }
            else if (clients[i]->check_all())
            {
                msge = ":" + clients[i]->get_nick_name() + "!~d@197.230.30.146 NICK :" + clients[i]->get_cmd(1) + "\r\n";
                send_reply(clients[i]->get_socket_fd(), msge);
            }
            clients[i]->set_nick_name(clients[i]->get_cmd(1));
        }
        else
        {
            msge = server_prefix + "433" + " * :Nickname is already in use\r\n";
            send_reply(clients[i]->get_socket_fd(), msge);
        }
    }
}

void Server::parse_user(int i)
{
    std::string  msge;
    if (clients[i]->check_all())
    {
        msge = server_prefix + "462 " + clients[i]->get_nick_name() + " :You may not reregister\r\n";
        send_reply(clients[i]->get_socket_fd(), msge);
    }
    else if (clients[i]->get_buffer_size() < 5)
    {
        
        msge = server_prefix + "461 " +  "USER :Not enough parameters\r\n";
        send_reply(clients[i]->get_socket_fd(), msge);
    }
    else
        clients[i]->set_user_infos();
}

void Server::try_to_auth(int i)
{
    std::string _n = clients[i]->get_nick_name();
    if (clients[i]->check_message())
    {
        std::string msge = server_prefix + "462 " + clients[i]->get_nick_name() + " :You may not reregister\r\n";
        send_reply(clients[i]->get_socket_fd(), msge);
    }
    else if (clients[i]->get_buffer_size() == 1)
    {
        std::string msge = server_prefix + "461 PASS: Not enough parameters\r\n";
        send_reply(clients[i]->get_socket_fd(), msge);
    }
    else if (clients[i]->get_buffer_size() > 1)
    {
        if (clients[i]->get_cmd(1) == password)
            clients[i]->correct_pass();
        else
        {
            std::string msge = server_prefix + "464 " +  clients[i]->get_cmd(1) + " :Password incorrect\r\n";
            send_reply(clients[i]->get_socket_fd(), msge);
            clients[i]->wrong_pass();
        }
    }
}

void Server::handle_cmd(int i)
{
    clients[i]->parse_command();
    // std::cout << clients[i]->get_buffer() << std::endl;
    if (clients[i]->get_cmd(0) == "quit" || clients[i]->get_cmd(0) == "QUIT")
    {
        handle_quit_cmd(i);
        return ;
    }
    if (!clients[i]->check_pass() && (clients[i]->get_cmd(0) != "pass" && clients[i]->get_cmd(0) != "PASS"))
    {
        std::string msge = server_prefix + "451 :You have not registered\r\n";
        send_reply(clients[i]->get_socket_fd(), msge);
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
}
