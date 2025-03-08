#include "server.hpp"

void    Server::broadcast_msge(std::string msge)
{
    for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); it++)
    {
        if ((*it)->check_connection())
            send_reply((*it)->get_socket_fd(), msge);
    }
}
