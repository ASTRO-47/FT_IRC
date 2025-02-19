#include "server.hpp"


void Server::try_to_auth(int i)
{
    // std::cout << password <<  std::endl;
    if (clients[i]->get_buffer().rfind("PASS", 0) == 0)
    {
        std::string pass = clients[i]->get_buffer().substr(5);
        pass[pass.length() - 1] = '\0';
        std::cout << "parsed password :" <<  pass<<'"' <<password<< '"' << std::endl;
        if (pass == password)
            std::cout << "correct passwor\n";
        // else
        //     std::cout << "incorrect password" << std::endl;
    }
}


void Server::handle_cmd(int i)
{
    if (!clients[i]->check_pass())
        try_to_auth(i);
    clients[i]->reset();
    // if (!_C->check_auth())
    //     try_to_auth(_C);
    // send(_C->get_socket_fd(), "you are not registered\n", 24, 0);
}
