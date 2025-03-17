#include "server.hpp"
#include "Channel.hpp"

void Server::handle_prv_msge(Client &_client)
{
    std::string _msge = _client.trim_message();
    std::string msge = ":" + _client.get_nick_name() +"!~" + _client.get_user_name() + "@" + _client.get_ip() +  " PRIVMSG " +_client.get_reciever()->get_nick_name() + " :" + _msge + CRLF;
    send_reply(_client.get_reciever()->get_socket_fd(), msge);
}

bool    Server::check_user(Client &_client)
{
    std::string _n = _client.get_cmd(1);

    for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); it++)
    {
        if ((*it)->get_nick_name() == _n && (*it)->check_all())
            return (_client.set_reciever((*it)), true);
    }
    if (channel_exists(_n))
    {
        std::string msge = ":" + _client.get_nick_name() +"!~" + _client.get_user_name() + "@" + _client.get_ip() +  " PRIVMSG " + _n + " :" + _client.trim_message() + CRLF;
        return (channelMap[_n].broadcastToAllMembers(msge, channelMap[_n], _client, 0), false);
    }
    std::string msge = server_prefix + "401 " + _n +  " :No such nick/channel" + CRLF;
    send_reply(_client.get_socket_fd(), msge);
    return false;
}
void    Server::change_nick_name(Client &_client)
{
    parse_nick(_client);
}

void    Server::handle_quit_cmd(Client &_C)
{
    std::string msge;
    if (_C.check_all())
        send_reply(_C.get_socket_fd(), CLIENT_QUIT(_C.get_nick_name(), _C.get_ip(), _C.get_user_name()));
    else if (!_C.check_all())
    {
        msge = ":!~@" + _C.get_ip() + " QUIT " + CRLF;
        send_reply(_C.get_socket_fd(), msge);
    }
    _C.disconnected();
}

bool Server::check_command(Client &_client)
{
    std::string msge;

    std::string input = _client.get_cmd(0);
    toLower(input);
    size_t buffer_size = _client.get_buffer_size();
    if (input == "quit")
        return (handle_quit_cmd(_client), true);
    if (input == "nick")
        return (change_nick_name(_client), true);
    else if (input == "privmsg")
    {
        if (buffer_size == 1)
        {
            msge = server_prefix + "411 " + _client.get_nick_name() + " :No recipient given (PRIVMSG)" + CRLF;
            send_reply(_client.get_socket_fd(), msge);
            return true;
        }
        if (buffer_size == 2)
        {
            msge = server_prefix + "412 " + _client.get_nick_name() + " :No text to send" + CRLF;
            send_reply(_client.get_socket_fd(), msge);
            return true;
        }
        if (!check_user(_client))
            return true;
        handle_prv_msge(_client);
        return true;
    }
    return false;
}
