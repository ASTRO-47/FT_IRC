#include "server.hpp"
#include "Channel.hpp"

void Server::handle_prv_msge(Client &_C)
{
    _C.trim_message();
    std::string msge = ":" + _C.get_nick_name() +"!~" + "@197.23.30.146" +  " PRIVMSG " +_C.get_reciever()->get_nick_name() + " :" + _C.get_message() + CRLF;
    send_reply(_C.get_reciever()->get_socket_fd(), msge);
}

bool    Server::check_user(Client &_C)
{
    std::string _n = _C.get_cmd(1);

    for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); it++)
    {
        if ((*it)->get_nick_name() == _n && (*it)->check_all())
        {
            _C.set_reciever((*it));
            return true;
        }
    }
    std::string msge = server_prefix + "402 " + _n +  " :No such nick/channel" + CRLF;
    send_reply(_C.get_socket_fd(), msge);
    return false;
}
void    Server::change_nick_name(Client &_C)
{
    parse_nick(_C);
}


void    Server::handle_quit_cmd(Client &_C)
{
    std::string msge;
    msge = ":";
    if (_C.check_all())
        msge += _C.get_nick_name();
    else
        msge += "*";
    msge += "!~f@197.230.30.146 QUIT :Client Quit" + CRLF;
    send_reply(_C.get_socket_fd(), msge);
    // removeUserFromChannels(_C); // fkr flblan dial tb9a channel bla operator etc..., hhhh kharya dyal de7k (by imad)
    msge = "ERROR :Closing Link: 197.230.30.146 (Client Quit)" + CRLF;
    send_reply(_C.get_socket_fd(), msge);
    close(_C.get_socket_fd());
    _C.disconnected();
}

bool Server::check_command(Client &_C)
{
    std::string msge;

    std::string input = _C.get_cmd(0);
    toLower(input);
    Client &client = _C;
    size_t buffer_size = _C.get_buffer_size();
    if (input == "nick")
        return (change_nick_name(_C), true);
    else if (input == "privmsg")
    {
        if (buffer_size == 1)
        {
            msge = server_prefix + "411 " + _C.get_nick_name() + " :No recipient given (PRIVMSG)" + CRLF; // need to fix something  in parsing
            send_reply(_C.get_socket_fd(), msge);
            return true;
        }
        if (buffer_size == 2)
        {
            msge = server_prefix + "412 " + _C.get_nick_name() + " :No text to send" + CRLF;
            send_reply(_C.get_socket_fd(), msge);
            return true;
        }
        if (!check_user(_C))
        {
            client.reset();
            return true;
        }
        handle_prv_msge(_C);
        return true;
    }
    return false;
}

// crash on join multiple times
