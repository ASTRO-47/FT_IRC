#include "server.hpp"

void f()
{
    system("leaks ircserv");
}

int main(int ac , char *av[])
{
    // atexit(f);

    char ip4[INET_ADDRSTRLEN];  // space to hold the IPv4 string
struct sockaddr_in sa;      // pretend this is loaded with something

inet_ntop(AF_INET, &(sa.sin_addr), ip4, INET_ADDRSTRLEN);

printf("The IPv4 address is: %s\n", ip4);


// IPv6:

char ip6[INET6_ADDRSTRLEN]; // space to hold the IPv6 string
struct sockaddr_in6 sa6;    // pretend this is loaded with something

inet_ntop(AF_INET6, &(sa6.sin6_addr), ip6, INET6_ADDRSTRLEN);

printf("The address is: %s\n", ip6);
    // int integer;
    // if (ac != 2)
    //     return (std::cerr << "invalid number of args\n", 1);
    // Server server;
    // try
    // {
    //     server.server_setup(av[1], "hello");
    //     server.multiplexing_func();
    // }

    // catch (std::exception &e)
    // {
    //     std::cout << "ERROR: " << e.what() << std::endl;
    // }

    // return 0;
}
