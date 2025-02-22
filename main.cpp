#include "server.hpp"

void f()
{
    system("leaks ircserv");
}


class test
{
    public:
        int h;
        test(int i)
        {
            h = i;
        }
};


int main(int ac , char *av[])
{
    test *p = new test(0);
    test *p1 = new test(1);
    test *p2 = new test(2);
    test *p3 = new test(3);
    std::vector<test *> arr;
    arr.push_back(p);
    arr.push_back(p1);
    arr.push_back(p2);
    arr.push_back(p3);

    // arr.erase(p2);
    // atexit(f);
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