#include "server.hpp"

bool Server::taken_nick_name_1(std::string _n) const
{
    for (std::vector<Client *>::const_iterator it = clients.begin(); it != clients.end(); it++)
    {
        if ((*it)->get_nick_name() == _n)  // Ensure the check is active
            return false;
    }
    return true;
}

bool    Server::check_nick_name(Client &_client)
{
    std::string nick = _client.get_cmd(1);

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

void Server::parse_nick(Client &_client)
{
    std::string msge;
    if (_client.get_buffer_size() == 1) // add the nick name is in use
    {
        msge = server_prefix + "431 :No nickname given" + CRLF;
        send_reply(_client.get_socket_fd(), msge);
    }
    else
    {
        if (!check_nick_name(_client))
        {
            std::string msge = server_prefix +  "431 * " + _client.get_cmd(1) +" :Erroneus nickname" + CRLF;
            send_reply(_client.get_socket_fd(), msge);
        }
        else if (taken_nick_name_1(_client.get_cmd(1)))
        {
            if (!_client.check_all() && _client.check_nick())
            {
                if (_client.check_first_nick())
                {
                    msge = _client.get_nick_name() + "!@ NICK :" + _client.get_cmd(1) + '\n';
                    send_reply(_client.get_socket_fd(), msge);
                }
            }
            else if (_client.check_all())
            {
                msge = ":" + _client.get_nick_name() + "!~" + _client.get_user_name() + "@" + _client.get_ip() + " NICK :" + _client.get_cmd(1) + CRLF;
                broadcast_msge(msge);
            }
            _client.set_nick_name(_client.get_cmd(1));
        }
        else
        {
            msge = server_prefix + "433" + " * :Nickname is already in use" + CRLF;
            send_reply(_client.get_socket_fd(), msge);
        }
    }
}

void Server::parse_user(Client &_client)
{
    std::string  msge;
    if (_client.check_all())
    {
        msge = server_prefix + "462 " + _client.get_nick_name() + " :You may not reregister" + CRLF;
        send_reply(_client.get_socket_fd(), msge);
    }
    else if (_client.get_buffer_size() < 5)
    {
        msge = server_prefix + "461 " +  "USER :Not enough parameters" + CRLF;
        send_reply(_client.get_socket_fd(), msge);
    }
    else
        _client.set_user_infos();
}

void Server::try_to_auth(Client &_client)
{
    std::string _n = _client.get_nick_name();
    if (_client.check_message())
    {
        std::string msge = server_prefix + "462 " + _client.get_nick_name() + " :You may not reregister" + CRLF;
        send_reply(_client.get_socket_fd(), msge);
    }
    else if (_client.get_buffer_size() == 1)
    {
        std::string msge = server_prefix + "461 PASS: Not enough parameters" + CRLF;
        send_reply(_client.get_socket_fd(), msge);
    }
    else if (_client.get_buffer_size() > 1)
    {
        if (_client.get_cmd(1) == password)
            _client.correct_pass();
        else
        {
            std::string msge = server_prefix + "464 " +  _client.get_cmd(1) + " :Password incorrect" + CRLF;
            send_reply(_client.get_socket_fd(), msge);
            _client.wrong_pass();
        }
    }
}

void Server::handle_cmd(Client &_client)
{
    _client.parse_command();
    if (!_client.get_buffer_size())
        return ;
    // std::cout << _client.get_buffer() << "]" << std::endl;
    if (_client.get_cmd(0) == "quit")
    {
        handle_quit_cmd(_client);
        return ;
    }
    if (!_client.check_pass() && _client.get_cmd(0) != "pass")
    {
        std::string msge = server_prefix + "451 * :You have not registered" + CRLF;
        send_reply(_client.get_socket_fd(), msge);
        return ;
    }
    if (_client.get_cmd(0) == "pass")
        try_to_auth(_client);
    if (_client.get_cmd(0) == "nick")
        parse_nick(_client);
    if (_client.get_cmd(0) == "user")
        parse_user(_client);
    if (_client.check_all() && !_client.check_message())
        registration_msge(_client);
}
