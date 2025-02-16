#include "server.hpp"

int main(int ac , char *av[])
{
    if (ac != 2)
        return (std::cerr << "invalid number of args\n", 1);
    Server server;
    try
    {
        server.server_setup(av[1]);
        server.wait_connections();
    }
    catch (std::exception &e)
    {
        std::cout << "ERROR: " << e.what() << std::endl;
    }
    // server.sock_addr.sin_port = htons(36895);
    // server.sock_addr.sin_family = AF_INET6;
    // server.sock_addr.sin_addr.s_addr = INADDR_ANY;
    // std::cout << server.sock_addr.sin_family << std::endl;
    return 0;
}