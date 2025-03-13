#include "server.hpp"

void Server::taken_nick_name(Client &_C)
{
    std::string _n = _C.get_nick_name();
    
    for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); it++)
    {
        if ((*it)->get_nick_name() == _n && !(*it)->check_all())
        {
            std::string msge = "ERROR :Closing Link: " + _C.get_nick_name() + " by :ft_irc (Overridden by other sign on)" + CRLF;
            send_reply(_C.get_socket_fd(), msge);
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

bool    Server::check_nick_name(Client &_C)
{
    std::string nick = _C.get_cmd(1);

    if (!nick.length() || nick.length() > MAX_NICK_LEN)
        return false;
    if (!isalpha(nick[0]))
        return false;
    for (size_t i = 1; i < nick.length() ; i++) 
    {
        if (!isalnum(nick[i]) && nick[i] != '-' && nick[i] != '[' && nick[i] != ']' && nick[i] != '\\' && nick[i] != '_')
            return false;
    }
    return true;
}

void Server::parse_nick(Client &_C)
{
    std::string msge;
    if (_C.get_buffer_size() == 1) // add the nick name is in use
    {
        msge = server_prefix + "431 :No nickname given" + CRLF;
        send_reply(_C.get_socket_fd(), msge);
    }
    else
    {
        if (!check_nick_name(_C))
        {
            std::string msge = server_prefix +  "431 * " + _C.get_cmd(1) +" :Erroneus nickname" + CRLF;
            send_reply(_C.get_socket_fd(), msge);
        }
        else if (taken_nick_name_1(_C.get_cmd(1)))
        {
            if (!_C.check_all() && _C.check_nick())
            {
                if (_C.check_first_nick())
                {
                    msge = _C.get_nick_name() + "!@ NICK :" + _C.get_cmd(1) + '\n';
                    send_reply(_C.get_socket_fd(), msge);
                }
                // send info to all the joined channels that the nik is changed
            }
            else if (_C.check_all())
            {
                msge = ":" + _C.get_nick_name() + "!~d@197.230.30.146 NICK :" + _C.get_cmd(1) + CRLF;
                send_reply(_C.get_socket_fd(), msge);
            }
            _C.set_nick_name(_C.get_cmd(1));
        }
        else
        {
            msge = server_prefix + "433" + " * :Nickname is already in use" + CRLF;
            send_reply(_C.get_socket_fd(), msge);
        }
    }
}

void Server::parse_user(Client &_C)
{
    std::string  msge;
    if (_C.check_all())
    {
        msge = server_prefix + "462 " + _C.get_nick_name() + " :You may not reregister" + CRLF;
        send_reply(_C.get_socket_fd(), msge);
    }
    else if (_C.get_buffer_size() < 5)
    {
        
        msge = server_prefix + "461 " +  "USER :Not enough parameters" + CRLF;
        send_reply(_C.get_socket_fd(), msge);
    }
    else
        _C.set_user_infos();
}

void Server::try_to_auth(Client &_C)
{
    std::string _n = _C.get_nick_name();
    if (_C.check_message())
    {
        std::string msge = server_prefix + "462 " + _C.get_nick_name() + " :You may not reregister" + CRLF;
        send_reply(_C.get_socket_fd(), msge);
    }
    else if (_C.get_buffer_size() == 1)
    {
        std::string msge = server_prefix + "461 PASS: Not enough parameters" + CRLF;
        send_reply(_C.get_socket_fd(), msge);
    }
    else if (_C.get_buffer_size() > 1)
    {
        if (_C.get_cmd(1) == password)
            _C.correct_pass();
        else
        {
            std::string msge = server_prefix + "464 " +  _C.get_cmd(1) + " :Password incorrect" + CRLF;
            send_reply(_C.get_socket_fd(), msge);
            _C.wrong_pass();
        }
    }
}

void Server::handle_cmd(Client &_C)
{
    _C.parse_command();
    if (!_C.get_buffer_size())
        return ;
    if (_C.get_cmd(0) == "quit" || _C.get_cmd(0) == "QUIT")
    {
        handle_quit_cmd(_C);
        return ;
    }
    if (!_C.check_pass() && (_C.get_cmd(0) != "pass" && _C.get_cmd(0) != "PASS"))
    {
        std::string msge = server_prefix + "451 :You have not registered" + CRLF;
        send_reply(_C.get_socket_fd(), msge);
        return ;
    }
    if (_C.get_cmd(0) == "pass" || _C.get_cmd(0) == "PASS")
        try_to_auth(_C);
    if (_C.get_cmd(0) == "nick" || _C.get_cmd(0) == "NICK")
        parse_nick(_C);
    if (_C.get_cmd(0) == "user" || _C.get_cmd(0) == "USER")
        parse_user(_C);
    if (_C.check_all() && !_C.check_message())
        registration_msge(_C);
}
