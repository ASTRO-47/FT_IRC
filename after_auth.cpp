#include "server.hpp"
#include "Channel.hpp"

void Server::handle_prv_msge(int i)
{
    clients[i]->trim_message();
    std::string msge = ":" + clients[i]->get_nick_name() +"!~" + "@197.23.30.146" +  " PRIVMSG " +clients[i]->get_reciever()->get_nick_name() + " :" + clients[i]->get_message() + CRLF;
    send_reply(clients[i]->get_reciever()->get_socket_fd(), msge);
}

bool    Server::check_user(int i)
{
    std::string _n = clients[i]->get_cmd(1);

    for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); it++)
    {
        if ((*it)->get_nick_name() == _n && (*it)->check_all())
        {
            clients[i]->set_reciever((*it));
            return true;
        }
    }
    std::string msge = server_prefix + "402 " + _n +  " :No such nick/channel" + CRLF;
    send_reply(clients[i]->get_socket_fd(), msge);
    return false;
}
void    Server::change_nick_name(int i)
{
    parse_nick(i);
    //broadcast to all channels
}


// when we change the nick kan3tabro b7ala user jdid 
// bghiti tkhdm bl exceptions khs dir try catch ajmi
void    Server::handle_quit_cmd(int i)
{
    std::string msge;
    msge = ":";
    if (clients[i]->check_all())
        msge += clients[i]->get_nick_name();
    else
        msge += "*";
    msge += "!~f@197.230.30.146 QUIT :Client Quit" + CRLF;
    send_reply(clients[i]->get_socket_fd(), msge);
    msge = "ERROR :Closing Link: 197.230.30.146 (Client Quit)" + CRLF;
    send_reply(clients[i]->get_socket_fd(), msge);
    close(clients[i]->get_socket_fd());
    clients[i]->disconnected();
}

bool Server::check_command(int i)
{
    std::string msge;

    std::string input = clients[i]->get_cmd(0);
    toLower(input);
    Client &client = *clients[i];
    size_t buffer_size = clients[i]->get_buffer_size();
    if (input == "nick")
        return (change_nick_name(i), true);
    else if (input == "privmsg")
    {
        if (buffer_size == 1)
        {
            msge = server_prefix + "411 " + clients[i]->get_nick_name() + " :No recipient given (PRIVMSG)" + CRLF; // need to fix something  in parsing
            send_reply(clients[i]->get_socket_fd(), msge);
            return true;
        }
        if (buffer_size == 2)
        {
            msge = server_prefix + "412 " + clients[i]->get_nick_name() + " :No text to send" + CRLF;
            send_reply(clients[i]->get_socket_fd(), msge);
            return true;
        }
        if (!check_user(i))
        {
            client.reset();
            return true;
        }
        handle_prv_msge(i);
        return true;
    }
    return false;
}

// crash on join multiple times