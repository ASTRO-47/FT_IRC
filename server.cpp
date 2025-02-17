#include "server.hpp"

Server::Server() {}

// bool check_numeric(std::string m)
// {
//     for (size_t i = 0; i , m.length();i++)
//     {
//         if (!std::isdigit(m[i]))
//             return false;
//     }
//     return true;
// }

void Server::server_setup(std::string _port, std::string passwd)
{
    // check the password policy
    password = passwd;
    char *checker = NULL;
    port = std::strtod(_port.c_str(), &checker);
    if (checker[0] != '\0')
        throw std::runtime_error("invalid port format");

    // need to check the port chosen

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) 
        throw std::runtime_error("failed to create socket");
    fcntl(server_socket, F_SETFL, O_NONBLOCK);
    sock_addr.sin_family = AF_INET; //  select the ipv4 protocols
    sock_addr.sin_addr.s_addr = INADDR_ANY; // chose the network interfaces will listen on
    sock_addr.sin_port = htons(port); // the port will listen on
    if (bind(server_socket, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) < 0)
    {
        close (server_socket);
        throw std::runtime_error("bind failed");
    }
    if (listen(server_socket, 5) < 0)
    {
        throw std::runtime_error("listen failed"); // try to print the errno
        close (server_socket);
    }
    struct pollfd new_pollfd;
    new_pollfd.fd = server_socket;
    new_pollfd.events = POLLIN; // tells pool which request will come
    new_pollfd.revents = 0; // the actuel request comes
    _poll_fds.push_back(new_pollfd);
    std::cout << "server listening on port: " << port << std::endl;
}

void Server::multiplexing_func()
{
    while (true)
    {
        int ready = poll(_poll_fds.data(), _poll_fds.size(), -1); // no blocking poll
        if (ready == -1)
            throw std::runtime_error("poll error");
        for (size_t i = 0; i < _poll_fds.size(); i++)
        {
            if (_poll_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) // check if a client cut off
            {
                std::cout << "hangup or error on fd " << _poll_fds[i].fd << std::endl;
                close(_poll_fds[i].fd);
                _poll_fds.erase(_poll_fds.begin() + i);
                i--;
            }

            if (_poll_fds[i].revents & POLLIN) // if a connection to the socket requested and its writing request
            {
                if (_poll_fds[i].fd == server_socket) // new connection, set same socket
                {
                    sockaddr_in client_addr;
                    socklen_t client_addr_size = sizeof(client_addr);
                    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
                    if (client_socket == -1)
                        throw std::runtime_error("request accepting failed");
                    struct pollfd new_pollfd;
                    new_pollfd.fd = client_socket   ;
                    new_pollfd.events = POLLIN;
                    new_pollfd.revents = 0;
                    _poll_fds.push_back(new_pollfd); // add the client to the socket
                    std::cout << "Client connected\n";
                }
                else
                {
                    char buffer[1024]; // change this later
                    int bytes = recv(_poll_fds[i].fd, buffer, 1023, 0);
                    if (bytes <= 0)
                    {
                        std::cout << "client disconnected\n";
                        close (_poll_fds[i].fd);
                        _poll_fds.erase(_poll_fds.begin() + i);
                        --i;
                    }
                    else
                    {
                        buffer[bytes - 1] = '\0';
                        std::cout << "recieved: " << buffer << std::endl;
                        send(_poll_fds[i].fd, "hello from astro server\n>", 26, 0);
                    }
                }
            }
        }
    }
}

Server::~Server()
{
    close(server_socket);
    close(client_socket);
}