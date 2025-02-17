#include "server.hpp"
#include <vector>
int main(int ac , char *av[])
{
    if (ac != 3)
        return (std::cerr << "invalid number of args\n", 1);
    Server server;
    try
    {
        server.server_setup(av[1], av[2]);
        server.wait_connections();
    }
    catch (std::exception &e)
    {
        std::cout << "ERROR: " << e.what() << std::endl;
    }

    return 0;
}
