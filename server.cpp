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
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) 
        throw std::runtime_error("failed to create socket");
    // fcntl(server_socket, F_SETFL, O_NONBLOCK);
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
    Client* poll_server = new Client();
    poll_server->add_server_to_poll(server_socket);
    clients.push_back(poll_server);
    _poll_fds.push_back(poll_server->get_socket_struct());
    std::cout << "server listening on port: " << port << std::endl;
}

void    Server::handle_new_client()
{
    Client *new_client = new Client;
    new_client->connect(server_socket);
    clients.push_back(new_client); // add the client to the socket
    _poll_fds.push_back(new_client->get_socket_struct());
    std::cout << "Client connected, on fd: "  <<  new_client->get_socket_fd()  << "\n";
}

void Server::handle_event_fd(int i)
{
    char buffer[1024]; // change this later
    int bytes = recv(clients[i]->get_socket_fd(), buffer, 1023, 0);
    if (bytes <= 0)
    {
        std::cout << "client disconnected\n";
        close (clients[i]->get_socket_fd());
        _poll_fds.erase(_poll_fds.begin() + i);
        --i;
    }
    else
    {
        buffer[bytes - 1] = '\0'; // trim the new line at the end
        std::cout << "recieved: " << buffer << std::endl;
        if (!strcmp(buffer, "halt"))
            throw std::runtime_error("server down");
        send(clients[i]->get_socket_fd(), "hello from astro server\n>", 26, 0);
    }
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
                if (_poll_fds[i].fd == server_socket) // new events is on the socket file desctiptor
                    handle_new_client();
                else
                    handle_event_fd(i);
            }
        }
    }
}

Server::~Server()
{
    for (size_t i = 0; i < clients.size(); i++)
    {
        if (clients[i]->get_socket_fd() != -1)
            close(clients[i]->get_socket_fd());
        delete clients[i];
    }
    clients.clear();

    if (server_socket != -1)
        close(server_socket);
}