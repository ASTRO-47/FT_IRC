#include "server.hpp"

void Server::handle_prv_msge(Client *C)
{
    C->trim_message();
    std::string msge = ":" + C->get_nick_name() +"!~" + C->get_nick_name() + "@197.23.30.146" +  " PRIVMSG " +C->get_reciever()->get_nick_name() + " :" + C->get_message() + '\n';
    send_private_message(C,  msge);
}

void Server::send_private_message(Client *sender, std::string msge)
{
    send_reply(sender->get_reciever()->get_socket_fd(), msge);

}

bool    Server::check_user(Client *C)
{
    std::string _n = C->get_cmd(1);

    for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); it++)
    {
        if ((*it)->get_nick_name() == _n && (*it)->check_all())
        {
            C->set_reciever((*it));
            return true;
        }
    }
    std::string msge = server_prefix + "402 " + _n +  " :No such nick/channel\n";
    send_reply(C->get_socket_fd(), msge);
    return false;
}

void    Server::change_nick_name(Client *C)
{
    parse_nick(C);
    //broadcast to all channels
}

void    Server::handle_cmd_1(Client *C)
{
    std::string msge;
    C->parse_command();
    if (C->get_cmd(0) == "nick" || C->get_cmd(0) == "NICK")
        change_nick_name(C);
    else if (C->get_cmd(0) == "privmsg" || C->get_cmd(0) == "PRIVMSG")
    {
        if (C->get_buffer_size() == 1)
        {
            msge = server_prefix + "411 " + C->get_nick_name() + " :No recipient given (PRIVMSG)\n";
            send_reply(C->get_socket_fd(), msge);
            C->reset();
            return ;
        }
        if (C->get_buffer_size() == 2)
        {
            msge = server_prefix + "412 " + C->get_nick_name() + " :No text to send\n";
            send_reply(C->get_socket_fd(), msge);
            C->reset();
            return ;
        }
        if (!check_user(C))
        {
            C->reset();
            return ;
        }
        handle_prv_msge(C);
    }
    else if (C->get_cmd(0) != "pong" && C->get_cmd(0) != "PONG" )
    {
        msge = server_prefix + "421 " + C->get_cmd(0) +  ": unkown command\n";
        send_reply(C->get_socket_fd(), msge);
    }
    C->reset();
}
