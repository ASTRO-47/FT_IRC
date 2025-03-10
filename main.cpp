#include "server.hpp"

void f()
{
    system("leaks ircserv");
}

int main(int ac , char *av[])
{
    // atexit(f);
    int integer;
    if (ac != 3)
        return (std::cerr << "invalid number of args\r\n", 1);
    Server server;
    try
    {
        server.server_setup(av[1], av[2]);
        server.multiplexing_func();
    }

    catch (std::exception &e)
    {
        std::cout << "ERROR: " << e.what() << std::endl;
    }

    return 0; // disconnected when ctrl + c the client reading from urandom
}
